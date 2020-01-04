#include "psf/comm/MLClient.h"
#include<striing>
#include <unordered_map>


class MLClientFactory {
 
 
 private:

  
  PSAgentMatrixMetaManager* matrix_meta_manager; // Matrix to meta 
 
  std::unordered_map<int , MLClient*> cached_clients; // matrix Id to MLClient
  
 
 public:
 
  /**
   * \brief return the singleton object
   */
  MLClientFactory():globalId(0){}

  MLClient* getClient(std::string name){

   int MatrixId = matrix_meta_manager->getMatrixId(name);
   
   if(MatrixId=-1){ // no matrix 

   MatrixId = matrix_meta_manager->addMatrix() // 

   Client= new MLClient(MatrixId); 
   
   (cached_clients.find(MatrixId)==cached_clients.end()) cached_clients[MatrixId]=Client;

 
   }


   return cached_clients[MatrixId];
   
  }



  





 }

  

};
