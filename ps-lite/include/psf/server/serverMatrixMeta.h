#ifndef SERVER_
#define SERVER_

#include"psf/psf/PSFunc.h"


// key 
struct ServerMatrixMeta{

psfType type;

int matrixId; // matrix id 
int partId;  //
int startRow; 
int endRow;
int  startCol;
int  endCol;

////////////for pushRow
int rowIndex;


ServerMatrixMeta(){ this->matrixId = -1;}

ServerMatrixMeta(psfType type, int matrixId, int partId, int startRow, int endRow, int startCol, int endCol, int rowIndex){

  this->type = type;
  this->matrixId = matrixId;
  this->partId = partId;
  this->startRow = startRow;
  this->endRow = endRow;
  this->startCol = startCol;
  this->endCol = endCol;
  this->rowIndex = rowIndex;

}


ServerMatrixMeta& operator=(const ServerMatrixMeta& other){

  this->type = other.type;
  this->matrixId = other.matrixId;
  this->partId = other.partId;
  this->startRow = other.startRow;
  this->endRow = other.endRow;
  this->startCol = other.startCol;
  this->endCol = other.endCol;
  this->rowIndex = other.rowIndex;
  return *this;

 }

};
#endif

