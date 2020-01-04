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



void init(){

 Start(0);

// if(!IsWorker()) assert(false);

}


void finalize(){

  Finalize(0,true);

}

bool isworker(){

  return IsWorker();

}

bool isserver(){

  return IsServer();

}







PYBIND11_MODULE(ps, m) {

    m.doc() = "ps module"; // optional module docstring
    m.def("init",&init,"Init");
    m.def("finalize",&finalize,"Finalize");
    m.def("isWorker",&isworker,"isworker");
    m.def("isServer",&isserver,"isserver");


}


