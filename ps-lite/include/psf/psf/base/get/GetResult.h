#ifndef _GETRESULT_
#define _GETRESULT_

#include "psf/psf/base/Result.h"
 
/**
 * The base class of get result.
 */
class GetResult:public Result {
  /*
   * Create a new GetResult.
   */
  public:
  
  GetResult():Result(ResponseType::SUCCESS) {
          
  }

  /**
   * Get a new GetResult.
   *
   * @param responseType SUCCESS or FAILED
   */
  GetResult(ResponseType responseType):Result(responseType) {}

};
#endif

