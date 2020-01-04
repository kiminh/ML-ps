from setuptools import setup
from setuptools import Extension

worker_module = Extension(name='worker',
                           sources=['worker.cpp'],
                           include_dirs=[
                                         "../../pybind11/include",
                                         "../..//include",
                                          "../../deps/include",
                                         ],
                           library_dirs=['../../build','../../deps/lib'],
                           libraries=['ps','protobuf','protobuf-lite','zmq','protoc'],
                           extra_compile_args=["-std=c++11"],
                           )
server_module = Extension(name='server',
                           sources=['server.cpp'],
                           include_dirs=[
                                         "../../pybind11/include",
                                         "../../include",
                                          "../../deps/include",
                                         ],
                           library_dirs=['../../build','../../deps/lib'],
                           libraries=['ps','protobuf','protobuf-lite','zmq','protoc'],
                           extra_compile_args=["-std=c++11"],
                           )
ps_module = Extension(name='ps',
                           sources=['ps.cpp'],
                           include_dirs=[
                                         "../../pybind11/include",
                                         "../../include",
                                          "../../deps/include",
                                         ],
                           library_dirs=['../../build','../../deps/lib'],
                           libraries=['ps','protobuf','protobuf-lite','zmq','protoc'],
                           extra_compile_args=["-std=c++11"],
                           )
setup(ext_modules=[ps_module, server_module,worker_module])
