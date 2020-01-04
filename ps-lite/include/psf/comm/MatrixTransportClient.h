



class MatrixTransportClient{ 

private:







 public:
  

 addToGetQueueForServer(){





 }


 Future<PartitionGetResult> get(int requestId, GetFunc func,
      PartitionGetParam partitionGetParam) {

		  
    ParameterServerId serverId =
        PSAgentContext.get().getMatrixMetaManager().getMasterPS((partitionGetParam.getPartKey()));
		

    GetUDFRequest request =
        new GetUDFRequest(requestId, partitionGetParam.getPartKey(), func.getClass().getName(),
            partitionGetParam);

    //LOG.debug("get request=" + request);
    //FutureResult<PartitionGetResult> future = new FutureResult<>();
    //requestToResultMap.put(request, future);
    
    addToGetQueueForServer(serverId, request);
	
    startGet();

    return future;

  }
  
};
