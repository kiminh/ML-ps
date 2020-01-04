#ifndef _PARTITIONER_
#define _PARTITIONER_

#include "psf/Matrix/MatrixContext.h"
#include <vector>
#include "psf/server/PartitionMeta.h"


/**
 * Matrix partitioner interface.
 */
class Partitioner {
  
/**
   * Init matrix partitioner
   *
   * @param mContext matrix context
   * @param conf
   */
  void init(MatrixContext mContext, Configuration conf)=0;

  /**
   * Generate the partitions for the matrix
   *
   * @return the partitions for the matrix
   */
  std::vector<PartitionMeta> getPartitions()=0;

  /**
   * Assign a matrix partition to a parameter server
   *
   * @param partId matrix partition id
   * @return parameter server index
   */
  int assignPartToServer(int partId)=0;

}

#endif

