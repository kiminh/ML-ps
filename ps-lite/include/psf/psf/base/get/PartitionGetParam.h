#ifndef _PARTITION_GET_PARAM_H
#define _PARTITION_GET_PARAM_H

#include "psf/psf/base/PartitionKey.h"
 
class PartitionGetParam{
  /**
   * matrix Id
   */
  protected:
  
  int matrixId;

  /**
   * partition key
   */

  PartitionKey partKey;


public:


  /**
   * Creates a new partition parameter.
   *
   * @param matrixId the matrix id
   * @param partKey  the part key
   */
  PartitionGetParam(int matrixId, PartitionKey partKey) {
    this->matrixId = matrixId;
    this->partKey = partKey;
  }

  /**
   * Creates a new partition parameter.
   */
  PartitionGetParam() {
    this->matrixId = -1;
    this->partKey = PartitionKey(); // default constructor
  }



 

  /**
   * Gets matrix id.
   *
   * @return the matrix id
   */
  int getMatrixId() {
    return matrixId;
  }

  /**
   * Sets matrix id.
   *
   * @param matrixId the matrix id
   */
   void setMatrixId(int matrixId) {
    this->matrixId = matrixId;
  }

  /**
   * Gets part key.
   *
   * @return the part key
   */
   PartitionKey getPartKey() {
    return partKey;
  }

  /**
   * Sets partition key.
   *
   * @param partKey the partition key
   */
   void setPartKey(PartitionKey partKey) {
    this->partKey = partKey;
  }

 
 };

#endif 
