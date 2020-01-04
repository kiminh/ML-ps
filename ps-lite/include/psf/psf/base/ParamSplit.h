#ifndef _PARAMSPLIT_
#define _PARAMSPLIT_

#include "psf/psf/base/get/PartitionGetParam.h"

/**
 * The Parameter split.
 */
 
 class ParamSplit{
	 

  /**
   * Split list.
   *
   * @return the list
   */
 
 virtual std::vector<PartitionGetParam> split()=0;


};
#endif

