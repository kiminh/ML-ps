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



void startServer() {

  if (!IsServer()) {
    return;
  }

  auto server = new KVServer<float>(0);
  server->set_request_handle(KVServerMLHandle<float>());
  RegisterExitCallback([server](){ delete server; });

}




PYBIND11_MODULE(server, m) {

    m.doc() = "server module"; // optional module docstring
    m.def("startServer",&startServer,"startServer");


}


