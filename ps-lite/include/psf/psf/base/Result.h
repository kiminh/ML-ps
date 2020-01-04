#ifndef _RESULT
#define _RESULT

enum ResponseType {
  SUCCESS=0, FAILED
};


class Result {
  
public :
  
  ResponseType responseType;

  Result(ResponseType responseType) {
    this->responseType = responseType;
  }

  ResponseType getResponseType() {
    return responseType;
  }
};
#endif
 
