/**
 * @file structures.c
 * @author Nicolas CIBULKA - Kevin BERNARD - Aelien MOUBECHE 
 * @brief this file is the .h used to manipulate the pagination block 
 * @version 0.1
 * @date 2021-03-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */


/**
 * @brief The structure for a pagination Unit
 * 
 */
typedef struct {
    int isFree;         //!< say if the pagination reference an allocated Zone
    int size;           //!< size of the allocated Zone
    void* zoneStart;    //!< pointer to the beginning of the allocated Zone
    void* zoneEnd;      //!< pointer to the end of the allocated Zone
}PaginationUnit;

/**
 * @brief The structure for the Data Ribbon
 * 
 */
typedef struct{
    int size;           //!< size of the array that contains data
    int usedSize;       //!< size used of the array
    void** ribbon;      //!< array that contains the data
    int* allocatedMap;  //!< Array that say if an element of the ribbon is allocated (1) or not(0)
}DataRibbon;

// ----- PaginationUnit Functions -----

PaginationUnit initPaginationUnit();

void resetPaginationUnit(PaginationUnit* unit);

// ----- DataRibbon Functions -----

DataRibbon initDataRibbon(int size);

void freeDataRibbon(DataRibbon* ribbon);


