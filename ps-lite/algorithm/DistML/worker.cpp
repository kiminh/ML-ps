#include<iostream>
#include<pybind11/pybind11.h>
#include<pybind11/numpy.h>
#include<vector>
#include"psf/client/client.h"
#include "ps/ps.h"
#include<cassert>
#include<psf/psf/PSFunc.h>
#include<psf/server/serverMatrixMeta.h>
#include<dmlc/logging.h>
#include<ps/internal/postoffice.h>
#include"ps/base.h"


using namespace ps;

namespace py = pybind11;






int Ele_size(py::buffer_info& buf){

      int total = 1;
      for(int i = 0 ; i< buf.shape.size();i++) total*= buf.shape[i];
      return total;
}


ServerMatrixMeta GenServerMatrixMeta(psfType type, int matrixId, py::buffer_info& buf){

         switch(type){
           
           case psfType::PushAll:
              {
              
               int startRow=0,startCol=0, endRow =-1, endCol=-1, rowIndex = -1 ,partId = 0 ;

               if(buf.shape.size()==1){
                    startRow = 0 ;
                    startCol = 0;
                    endRow = 1;
                    endCol = buf.shape[0];

              }else if (buf.shape.size()==2){
                    
                    startRow = 0;
                    startCol = 0;
                    endRow = buf.shape[0];
                    endCol = buf.shape[1];

              }

               ServerMatrixMeta meta(type,matrixId,partId, startRow,endRow, startCol, endCol ,rowIndex);
               return meta;


              }
             break;

           default:

             LOG(ERROR)<<"unsupported psfType";
             ServerMatrixMeta meta;
             return meta;

        }

}

ReqMatrixMeta GenReqMatrixMeta(psfType type, int matrixId){


  switch(type){

   case psfType::PullAll:
     {
      
         ReqMatrixMeta meta(type,matrixId,-1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1 );
         return meta;

    }
   break;

  default:
      LOG(ERROR)<<"unsupported psftype";
      ReqMatrixMeta meta;
      return meta;
 }

}

Client<float> client(0,0);

void barrier_worker();

void  pushAll(py::array_t<float>& input, int matrixId){

    py::buffer_info buf = input.request();
    float* ptr = (float*)buf.ptr;
    CHECK_LE(buf.shape.size(),2); // buf.shape.size() <=2
    int size = Ele_size(buf);
    std::vector<float> vals(ptr,ptr+size);

    //generate matrix meta    
    ServerMatrixMeta meta = GenServerMatrixMeta(psfType::PushAll,matrixId, buf);

    // for LR , the dimension is 1 
    // for(int i = 0 ; i < buf.shape[0];i++ ) vals.push_back(ptr[i]);
    // Client<float> client(0,0);
    
    client.Wait(client.Push(vals,meta));
    barrier_worker();
   // return ts;
}


py::array_t<float> pullAll(int matrixId, py::array_t<float>& param){

 ReqMatrixMeta meta = GenReqMatrixMeta(psfType::PullAll, matrixId);

 std::vector<float> ret;
 std::vector<int> lens;
 client.Wait(client.Pull(ret,lens,meta));
 
 barrier_worker();
 auto output = py::array_t<float>(param.request().size);

// std::cout<<"output size"<<output.request().shape.size()<< output.request().shape[0]<<std::endl;

 py::buffer_info buf = output.request();
 float* ptr =(float*) buf.ptr;

 for(int i=0 ; i<ret.size();i++) 
     ptr[i] = ret[i];

 return output;

}


void wait(int timestamp){

    client.Wait(timestamp);

}

void barrier_worker(){
 
  Postoffice::Get()->Barrier(0,kWorkerGroup);

}




PYBIND11_MODULE(worker, m) {

    m.doc() = "worker module"; // optional module docstring
    m.def("pushAll",&pushAll,"a function pushAll to ps");
    m.def("pullAll",&pullAll,"a function pullAll from ps");
    m.def("wait",&wait,"wait timestamp");
    m.def("barrier_worker",&barrier_worker,"barrier");


}


