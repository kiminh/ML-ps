#ifndef _GETFUNC_H
#define _GETFUNC_H
 
#include"psf/psf/base/get/GetParam.h"
#include"psf/psf/base/get/PartitionGetParam.h"
#include"psf/psf/base/get/PartitionGetResult.h"


class GetFunc{

private:
GetParam param;

  /**
   * PS context
   */
//  protected:
  
//  PSContext psContext;

public:



  /**
   * Set PS context
   *
   * @param psContext ps context
   */
//  public void setPsContext(PSContext psContext) {
//    this.psContext = psContext;
//  }



GetFunc(GetParam param) {
     this->param = param;
}
GetParam getParam() {return param;} // 
virtual PartitionGetResult partitionGet(PartitionGetParam partParam); // server
virtual GetResult merge(std::vector<PartitionGetResult> partResults); // worker

};

#endif

