#ifndef _GETPARAM_H_
#define _GETPARAM_H_

#include "psf/psf/base/ParamSplit.h"

class GetParam :public  ParamSplit { // ParamSplit is a interface , split
   
  /**
   * The Matrix id.
   */
  public:

  int matrixId;

  /**
   * Creates a new Get parameter.
   *
   * @param matrixId the matrix id
   */
 GetParam(int matrixId) {
  
  this->matrixId = matrixId;
  
  }

  GetParam() {
	  
    this->matrixId=-1;
	
  }
  

  /**
   * Gets matrix id.
   *
   * @return the matrix id
   */
  int getMatrixId() {
	  
    return matrixId;
  
  }

  std::vector<PartitionGetParam> split() override {

  // matrixID to key ID 
    std::vector<PartitionKey>&  parts = getPartitions(matrixId); // where to get key ids , 
    
	size_t size = parts.size();

    std::vector<PartitionGetParam> partParams;

    for (size_t i = 0; i < size; i++) {
		
      partParams.push_back(PartitionGetParam(matrixId, parts[i]));
    
	}

    return partParams;
  }

};
#endif

