#include "ps/kv_app.h"
#include "psf/Partition/RowPartition.h"
#include "psf/psf/PSFunc.h"
#include "ps/base.h"
#include <vector>
#include "psf/server/serverMatrixMeta.h"
#include "psf/client/ReqMatrixMeta.h"
#include<unordered_map>

using namespace ps;

template<typename Val>
class Client{

private:

 KVWorker<Val> kv;
 
 Partition<Val> par; 
 
 int globalId;

 std::unordered_map<int,std::vector<Key>> matrixToKey;
 std::unordered_map<int,std::unordered_map<int,Key>> matrixRowToKey;

public:


 Client(int app_id, int customer_id):kv(app_id,customer_id),globalId(0){

 //kv = KVWorker<Val>(app_id,customer_id);

 this->par = Partition<Val>();

 }

 std::vector<Key>& findKey(int matrixId,std::vector<int> ps, std::unordered_map<int,int> psRow){ // rowId to ps rank

   if(matrixToKey.find(matrixId)!=matrixToKey.end()) return matrixToKey[matrixId];

   std::vector<Key> new_key;
   const std::vector<Range>& ranges = Postoffice::Get()->GetServerKeyRanges();
   for(size_t i = 0 ;i< ps.size();i++)  new_key.push_back(ranges[ps[i]].begin()+globalId);
   matrixToKey[matrixId]=new_key;
    
   std::unordered_map<int,Key> newpsRow;
   for(auto & e: psRow){
     
        newpsRow[e.first] = ranges[e.second].begin()+globalId;

    }
   
   this->matrixRowToKey[matrixId] = newpsRow;

   globalId++;

   return matrixToKey[matrixId];
 }

  Key findRowKey(int matrixId, int rowId){

     
       return this->matrixRowToKey[matrixId][rowId]; 


   }

 



 int Push(std::vector<Val>& matrix, ServerMatrixMeta meta){

   psfType type = meta.type;
  
   switch(type){

   case psfType::PushAll:
    {
     
     std::vector<std::vector<float>> partitionMatrix;
     std::vector<ServerMatrixMeta> partitionMeta;
     this->par.RowPartition(matrix, meta,partitionMatrix,partitionMeta);
     std::vector<int> lens;
     for(size_t i = 0 ; i < partitionMatrix.size();i++) lens.push_back(partitionMatrix[i].size());

     /////////
//     std::cout<<"partitionMatrix size: "<<partitionMatrix.size()<<std::endl;
//     for(auto & e: partitionMeta) std::cout<< e.partId << std::endl;
      
     //////////

     int matrixId = meta.matrixId;
     const std::vector<Key>& keys = findKey(matrixId,this->par.MatrixToPs(matrixId),this->par.MatrixToPsRow(matrixId));
     // keys , vals
      int ts = kv.Push(keys,matrix,lens, partitionMeta);
      return ts;
     
     }

     break;

  case psfType::PushRow:

     {
      int matrixId = meta.matrixId;
      int rowId = meta.rowIndex;
      
      //int ps =  this->par.MatrixRowTops(matrixId, rowId); // RowPartition, row exit on one ps machine
      
      //const std::vector<Range>& ranges = Postoffice::Get()->GetServerKeyRanges();
      
      std::vector<int> lens{(int)matrix.size()};

      Key key = findRowKey(matrixId,rowId);
      
      std::vector<Key> keys{key};// for the purpose of send the update request to ps server , not to store it, so not need to generate a new key,     
      std::vector<ServerMatrixMeta> metas{meta};
      int ts = kv.Push(keys,matrix,lens,metas);
      return ts;
      
     }

    break;


  default:

    LOG(ERROR)<<"unsupporeted psfType";


  }
 }


  void Wait(int timestamp) { kv.Wait(timestamp); }


  //kv.Pull(keys,&ret,req,&lens_));
  
  int Pull(std::vector<Val>& vals, std::vector<int>& lens, ReqMatrixMeta req){
 
        psfType type = req.type;
        const std::vector<Range>& ranges = Postoffice::Get()->GetServerKeyRanges();
        
        switch(type){
       
         case psfType::GetRow:
          {
               int matrixId = req.matrixId;
               int rowId = req.rowIndex;
               int ps = this->par.MatrixRowToPs(matrixId,rowId);
               std::vector<Key> keys{ranges[ps].begin()};
               req.key = findRowKey(matrixId,rowId);
               std::vector<ReqMatrixMeta> reqs(1,req);
               
               int ts = kv.Pull(keys,&vals,reqs,&lens);
               return ts;

          }
           break;
          case psfType::PullAll:
             {

               int matrixId = req.matrixId;
               const std::vector<int>& ps = this->par.MatrixToPs(matrixId);
               const  std::vector<Key>& keys = findKey(matrixId,ps,this->par.MatrixToPsRow(matrixId));
               std::vector<ReqMatrixMeta> reqs(ps.size(),req);
               for(size_t i = 0 ; i < reqs.size();i++){
                     ReqMatrixMeta& meta = reqs[i];
                     meta.key = keys[i];
                }
               int ts = kv.Pull(keys,&vals,reqs,&lens);
               return ts;
             
            }
             break;
           case psfType::RowSum:
             {

               int matrixId = req.matrixId;
               int rowId = req.rowIndex;
               int ps = this->par.MatrixRowToPs(matrixId,rowId);
               std::vector<Key> keys{ranges[ps].begin()};
               req.key = findRowKey(matrixId,rowId);
               std::vector<ReqMatrixMeta> reqs(1,req);
               int ts = kv.Pull(keys,&vals,reqs,&lens);
               return ts;

             }
            break;

           case psfType::ColDot:
            {

                int matrixId1 = req.matrixId;
                int matrixId2 = req.matrixId2;
                const std::vector<int>& ps1 = this->par.MatrixToPs(matrixId1); // same for matrix2
                const std::vector<int>& ps2 = this->par.MatrixToPs(matrixId2); // same for matrix2

                const std::vector<Key>& keys1 = findKey(matrixId1,ps1,this->par.MatrixToPsRow(matrixId1));
                const std::vector<Key>& keys2 = findKey(matrixId2,ps2,this->par.MatrixToPsRow(matrixId2));
                std::vector<ReqMatrixMeta> reqs(ps1.size(),req);
            
                for(size_t i = 0 ;i < reqs.size();i++){
                     ReqMatrixMeta& meta = reqs[i];
                     meta.key = keys1[i];
                     meta.key2= keys2[i];           
               }
               
               for(auto e : keys1) std::cout<< e << std::endl;
               for(auto e : keys2) std::cout<< e << std::endl;

               int ts = kv.Pull(keys1,&vals,reqs,&lens); // use keys2 is ok , 
               return ts;

            }

            break;

           default:
               LOG(ERROR)<<"not supported psfType";


        }

  }

};

