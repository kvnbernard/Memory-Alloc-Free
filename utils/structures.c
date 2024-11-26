/**
 * @file structures.c
 * @author Nicolas CIBULKA - Kevin BERNARD - Aelien MOUBECHE 
 * @brief this file is the .c used to manipulate the header block 
 * @version 0.1
 * @date 2021-03-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "ipcTools.h"

// ----- PaginationUnit Functions -----

/**
 * @brief Initialise an empty PaginationUnit
 * 
 * @return PaginationUnit 
 */
PaginationUnit initPaginationUnit(){
    PaginationUnit unit;
    unit.isFree = 0;
    unit.size = 0;
    unit.zoneStart = NULL;
    unit.zoneEnd = NULL;
    return unit;
}

/**
 * @brief reset a given paginationUnit
 * 
 * @param unit : THe unit that will be reset
 */
void resetPaginationUnit(PaginationUnit* unit){
    unit->isFree = 0;
    unit->size = 0;
    unit->zoneStart = NULL;
    unit->zoneEnd = NULL;
}

// ----- DataRibbon Functions -----

/**
 * @brief Initialise the Data Ribbon that will contains all our values
 * 
 * @param size The size of the Ribbon
 * @return DataRibbon : the array that will contains all our data
 */
DataRibbon initDataRibbon(int size){
    DataRibbon ribbon;
    ribbon.size = size;
    ribbon.usedSize = 0;
    ribbon.ribbon = malloc(sizeof(void*) * size);
    ribbon.allocatedMap = malloc((sizeof(int) * size));
    for(int i = 0; i < size; i++){
        ribbon.allocatedMap[i] = 0;
    }
    return ribbon;
}

/**
 * @brief Free a given DataRibbon
 * 
 * @param ribbon : The Ribbon that will be released
 */
void freeDataRibbon(DataRibbon* ribbon){
    ribbon->size =0;
    ribbon->usedSize =0;
    free(ribbon->ribbon);
    free(ribbon->allocatedMap);
}