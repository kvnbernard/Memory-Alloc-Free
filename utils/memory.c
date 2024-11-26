/**
 * @file memory.c
 * @author Nicolas CIBULKA - Kevin BERNARD - Aelien MOUBECHE
 * @brief This file is the .c that allow to do all the operations of allocation/free on the memory
 * @version 0.1
 * @date 2021-03-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "memory.h"
#include "ipcTools.h"

/**
 * @brief The memory variable that will store all the allocated elements
 * 
 */
Memory memory;

/**
 * @brief Function to initiate the memory list
 * 
 * @param n Represent the size of the great structure that will contains all the allocated elements
 * @return An Integer that will be 0 if an error occured, or the size given in parameter if the operation succeed 
 */
int initMemory(int size){
    // initialise the mutex
    key_t key = ftok("/etc/passwd", 5);
    if(key == -1){
        return 0;
    }
    memory.mutex = mutalloc(key);

    // initialise the PaginationArray
    memory.paginationPage = malloc(sizeof(PaginationUnit)*size);
    if(memory.paginationPage == NULL){
        return 0;
    }
    // init each PaginationUnit
    for(int i = 0; i < size; i++){
        memory.paginationPage[i] = initPaginationUnit();
    }
    // Init the data Structure
    memory.dataArray = initDataRibbon(size);
    if(memory.dataArray.ribbon == NULL){
        return 0;
    }
    return size;
    
}   

/**
 * @brief This function will free the great Memory structure
 * 
 * @return An Integer that wille be the size that the function has got back, or -1 if an error occured
 */
int freeMemory(){
    int size = memory.dataArray.size;
    freeDataRibbon(&(memory.dataArray));
    free(memory.paginationPage);
    mutfree(memory.mutex);
    return size;
}

/**
 * @brief This function will do the allocation of a block, according to the size given in parameter
 * 
 * @param size The size of the data block we want
 * @return A void* which is the adress of the data block
 */
void* myAlloc(int size){
    // Start the critical section
    P(memory.mutex);
    
    // looking if the remaining size is enough for the asked size
    if(memory.dataArray.usedSize + size > memory.dataArray.size){
        // end of the critical section
        V(memory.mutex);
        return NULL;
    }
    
    // get the fist free pagination Unit for the allocation
    PaginationUnit *unit = getFirstPaginationUnit();
    if(unit == NULL){
        // end of critical section
        V(memory.mutex);
        return NULL;
    }

    // get the adress of the first element in the dataArray
    void* firstAdress = memory.dataArray.ribbon;
    // get the adress of the area allocated
    void* startBlock =  getBestFit(size);
    
    // verify if the allocated area exists
    if(startBlock == NULL){
        // end of critical section
        V(memory.mutex);
        return NULL;
    }

    // update of the allocatedMap
    int pos = startBlock - firstAdress;
    actualizeAllocatedMap(pos, size);

    // filling the PaginationUnit
    unit->isFree = 0;
    unit->size = size;
    unit->zoneStart = startBlock;
    unit->zoneEnd = startBlock+size;

    // increasing the usedSize
    memory.dataArray.usedSize += size;
    // end of critical section
    V(memory.mutex);
    // return the adress of the area allocated
    return unit->zoneStart;
}

/**
 * @brief The Function that free the memory area given in parameter 
 * 
 * @param p The pointer to the data block
 * @return an Integer the size of the memory removed
 */
int myFree(void *p)
{
    // Beginning of the critical section
    P(memory.mutex);
    // searching the PaginationUnit that store the adress we want to free
    for(int i = 0; i < memory.dataArray.size; i++){
        if(memory.paginationPage[i].zoneStart == p){
            // searching the index in the arry of the beginning of the area we want to free
            int size = memory.paginationPage[i].size;
            void* startMem = &memory.dataArray.ribbon[0];
            int startZone = startMem - memory.paginationPage[i].zoneStart;
            // refresh the allocatedMap
            resetAllocatedMap(startZone, size);
            // reset the paginationUnit
            resetPaginationUnit(&memory.paginationPage[i]);
            // modify the usedSize
            memory.dataArray.usedSize-=size;
            // end of critical section
            V(memory.mutex);
            // return the size
            return size;
        }
    }
    // if we arrive there, it means the adress given wasn't allocated
    V(memory.mutex);
    return -1;
}


/**
 * @brief Get the adress of the First free Pagination Unit 
 * 
 * @return PaginationUnit* 
 */
PaginationUnit* getFirstPaginationUnit(){
    for(int i = 0; i < memory.dataArray.size; i++){
        if(memory.paginationPage[i].isFree == 0){
            return &(memory.paginationPage[i]);
        }
        i++;
    }
    return NULL;
}

/**
 * @brief Get the adress of the first element of the dataArray that would have enough place after
 * This according to the best Fit Algorithm
 * @param size The needed size for the block 
 * @return void* : The adress of the first element of the block
 */
void* getBestFit(int size){
    // init useful variables
    int deltaSize = memory.dataArray.size + 1;
    // Default value of bestFit return : NULL
    void* adress = NULL;
    int cursor = 0;
    // if the memory is empty, the bestFit algorithm return is the first element of the memoryArray
    if(memory.dataArray.usedSize == 0){
        adress = memory.dataArray.ribbon[0];
    }
    // While the memory hasn't been fully analysed
    while(cursor < memory.dataArray.size){
        // if we find a non allocated space
        if(memory.dataArray.allocatedMap[cursor] == 0){
            int tempCursor = cursor;
            int tempSize = 0;
            // we look for the size of the non allocated area
            do{
                tempSize++;
                tempCursor++;  
            }while(memory.dataArray.allocatedMap[tempCursor] != 1 && tempCursor < memory.dataArray.size);

            // if the area analysed is enough big for the wanted area, and smaller than the current area
            if(tempSize > size && deltaSize > (tempSize - size)){
                // replacement of the adress we will return
                adress = &memory.dataArray.ribbon[cursor];
                deltaSize = tempSize - size;
            }
            // moving the cursor to the end of the analysed area
            cursor = tempCursor;
        }
        cursor++;
    }
    
    return adress;
}

/**
 * @brief This function will update the map of allocatedAdress, to help bestFit Algorithm later.
 * This will turn all the elements in the aimed area in 1 to say that those elements are allocoated
 * 
 * @param startPos : the starting position of the allocated area
 * @param size : the size of the allocated area
 */
void actualizeAllocatedMap(int startPos, int size){
    for(int i = startPos; i < startPos+size; i++){
        memory.dataArray.allocatedMap[i] = 1;
    }
}

/**
 * @brief This function will update the map of allocatedAdress, to help bestFit Algorithm later.
 * This will turn all the elements in the aimed area in 0 to say that those elements are now released
 * 
 * @param startPos : the starting position of the allocated area
 * @param size : the size of the allocated area
 */
void resetAllocatedMap(int startPos, int size){
    for(int i = startPos; i < startPos+size; i++){
        memory.dataArray.allocatedMap[i] = 0;
    }
}