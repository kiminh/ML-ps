#ifndef USER_REQUEST_ADAPTER_
#define USER_REQUEST_ADAPTER_

#include "psf/comm/MatrixTransportClient.h"
#include "psf/psf/base/get/GetParam.h"
#include "psf/psf/base/get/PartitionGetParam.h"
#include "psf/psf/base/get/GetResult.h"


class UserRequestAdapter{

  private:

  MatrixTransportClient* matrixClient;
	
  public:


    UserRequestAdapter(){

    matrixClient =  new MatrixTransportClient();

   }


 ~UserRequestAdapter(){

  if(matrixClient) delete matrixClient;

}
	
   //FutureResult<GetResult> get(GetFunc func){
   void get(GetFunc func){
		
    MatrixTransportClient matrixClient = MatrixTransportClient();
	
    GetParam param = func.getParam();

    // Split param use matrix partitons
    std::vector<PartitionGetParam> partParams = param.split();
    int size = partParams.size();

    GetPSFRequest request = new GetPSFRequest(func);

    int requestId = request.getRequestId();
    
	//FutureResult<GetResult> result = new FutureResult<>();
    //GetPSFResponseCache cache = new GetPSFResponseCache(size);

    requests.put(requestId, request);
    
	//requestIdToSubresponsMap.put(requestId, cache);
    //requestIdToResultMap.put(requestId, result);


    for (int i = 0; i < size; i++) {
		
      matrixClient->get(requestId, func, partParams[i]);
    }
    return result;
  }

};
#endif

