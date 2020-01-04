#include "psf/server/serverMatrixMeta.h"
#include "dmlc/logging.h"
#include "ps/internal/postoffice.h"
#include <vector>
#include "ps/range.h"

// Row based partition for a parameter matrix [startRow, endRow) [startCol, endCol)
// @param matrix the origin matrix value
// @param matrixmeta the origin matrixmeta
// @param PartitionMatrix the row base partitioned matrix value
// @param the meta for every partitioned matrix


using namespace ps;

template<typename Val>
class Partition{


private:


 std::unordered_map<int,std::vector<int> > MatrixToPS; // matrixId to Ps server rank 
 std::unordered_map<int,std::unordered_map<int,int>> MatrixToRowToPS; // matrixId->(RowIndex->PS) matrixId-> RowId->PS server rank 



public:


std::vector<int>& MatrixToPs(int matrixId){

 if(MatrixToPS.find(matrixId)!=MatrixToPS.end())
          return MatrixToPS[matrixId];
 else {LOG(ERROR)<<"matrixId not exist";}

}

int MatrixRowToPs(int matrixId, int rowId){
if(MatrixToRowToPS.find(matrixId)!=MatrixToRowToPS.end()){
       if( MatrixToRowToPS[matrixId].find(rowId)!=MatrixToRowToPS[matrixId].end())return MatrixToRowToPS[matrixId][rowId];
       else LOG(ERROR)<<"rowid not exit in matrix";
}
else {LOG(ERROR)<<"matrixId not exist";}
}

std::unordered_map<int,int>& MatrixToPsRow(int matrixId){


if(MatrixToRowToPS.find(matrixId)!=MatrixToRowToPS.end()){
   
        return MatrixToRowToPS[matrixId];
       
}
else {LOG(ERROR)<<"matrixId not exist";}

}

void RowPartition(const std::vector<Val>& matrix, const ServerMatrixMeta& matrixmeta, std::vector<std::vector<Val>> & PartitionMatrix, std::vector<ServerMatrixMeta>& PartitionMeta){


    size_t  matrixLen = matrix.size(); //


    int matrixId = matrixmeta.matrixId;
    psfType type = matrixmeta.type;
    CHECK_EQ(type,psfType::PushAll); // only PushAll use RowPartition
     


    //
    int startRow = matrixmeta.startRow;
    int startCol = matrixmeta.startCol;
    int endRow = matrixmeta.endRow;
    int endCol = matrixmeta.endCol;
    int elePerRow = endCol-startCol;
    int elePerCol = endRow-startRow;
    int eleTotal = elePerRow*elePerCol;
    CHECK_EQ((size_t)eleTotal,matrixLen);

    // compute the startRow and endRow for every server
    const std::vector<Range>& ranges = Postoffice::Get()->GetServerKeyRanges();
    size_t size = ranges.size();
    

    std::vector<int> RowNum(size,elePerCol/size);
    for(size_t i = 0 ; i< elePerCol%size ;i++) {
    RowNum[i]++;
   }
 

  int realSize=0;
  for(auto& e:RowNum) if(e!=0) realSize++;   
   

   //PartitionMatrix.resize(realSize);
   //PartitionMeta.resize(realSize);

   std::vector<int> startPos;
   startPos.push_back(startRow);
   for(int i = 1; i<realSize;i++) { int PrevEndRow = startPos[i-1]+RowNum[i-1]; startPos.push_back(PrevEndRow); }


   for(int i = 0 ; i <realSize ;i++){

    if(RowNum[i]!=0){

     // 
     MatrixToPS[matrixId].push_back(i);
     for(int j = 0 ; j <RowNum[i];j++) MatrixToRowToPS[matrixId][startPos[i]+j] = i;
  

    int startEle = (startPos[i]-startRow)*elePerRow;
    int endEle = startEle + RowNum[i]* elePerRow;

    typename  std::vector<Val>::const_iterator itStart  = matrix.begin();
    itStart+=startEle;
    typename  std::vector<Val>::const_iterator itEnd  = matrix.begin();
    itEnd+=endEle;

    std::vector<Val> parMatrix(itStart,itEnd);
    
    PartitionMatrix.push_back(parMatrix);

    ServerMatrixMeta parMeta(type, matrixId, i, startPos[i],startPos[i]+RowNum[i],startCol,endCol,-1); // ServerMatrixMeta(psfType type, int matrixId, int partId, int startRow, int endRow, int startCol, int endCol, int rowIndex)
    PartitionMeta.push_back(parMeta);

  }

 
}





    
}

};
