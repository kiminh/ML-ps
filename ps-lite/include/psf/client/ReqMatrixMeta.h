#ifndef REQ_
#define REQ_

#include "psf/psf/PSFunc.h"
#include "ps/base.h"

using namespace ps;

struct ReqMatrixMeta{

psfType type; // enum means gerow, RowSum, getcol, colsum

// for unary op, just use matriId and rowIndex or ColIndex, for binary op, if rowIndex and rowIndex2 are in the same matrix, matrixId == matrixId2, key == key2
int matrixId;
int matrixId2;

Key key;
Key key2;


///////////////////////////////// for row binary op

int rowIndex;
int rowIndex2;

/////////////////// for column binay op

int colIndex;
int colIndex2;

//// used for sort in worker end
////used in row psffunc///////

int startCol;
int endCol;

///////used in colum psfunc///

int startRow;
int endRow;

//////////////////////////////


ReqMatrixMeta():type(psfType::Other),matrixId(-1),matrixId2(-1),key(-1),key2(-1),rowIndex(-1),rowIndex2(-1),colIndex(-1),colIndex2(-1),startCol(-1),endCol(-1),startRow(-1),endRow(-1){}
ReqMatrixMeta(psfType type, int matrixId,int matrixId2,Key key, Key key2, int rowIndex, int rowIndex2,int colIndex, int colIndex2, int startRow, int endRow, int startCol, int endCol):type(type),matrixId(matrixId),matrixId2(matrixId2),key(key),key2(key2),rowIndex(rowIndex),rowIndex2(rowIndex2),colIndex(colIndex),colIndex2(colIndex2),startCol(startCol),\
endCol(endCol),startRow(startRow),endRow(endRow){}

ReqMatrixMeta(const ReqMatrixMeta& other){

   this->matrixId = other.matrixId;
   this->matrixId2= other.matrixId2;
   this->key = other.key;
   this->key2 = other.key2;
   this->rowIndex = other.rowIndex;
   this->rowIndex2 = other.rowIndex2;
   this->colIndex = other.colIndex;
   this->colIndex2=other.colIndex2;
   this->type = other.type;
   this->startRow = other.startRow;
   this->endRow = other.endRow;
   this->startCol = other.startCol;
   this->endCol = other.endCol;

}


void setStartRow(int startRow) {this->startRow = startRow;}

void setStartCol(int startCol) {this->startCol = startCol;}

void setEndRow(int endRow) {this->endRow = endRow;}

void setEndCol(int endCol) {this->endCol = endCol;}



};

#endif
