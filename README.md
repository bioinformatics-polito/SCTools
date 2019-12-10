# SCTools - Single-Cell utilities tools

SCTools is a suite of tools performing utility operations over
single-cell samples.

## Configure, build and install
SCTools is versioned using *git* and can be retrieved by typing
the following command on the command line:
```
git clone https://philae.polito.it/gitlab/biotools/sctools.git
```
or, if you have a private SSH key registered on *philae*:
```
git clone ssh://git@philae.polito.it:2222/biotools/sctools.git
```

SCTools is written in C++ and configured via CMake. For being
configured, it requires at least CMake version 3.7.2. Once the
project is downloaded from the remote *git* repository, the 
configuration process is triggered by
```
cd sctools
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
If the build process is successful, the executables can be 
retrieved from `sctools/build/apps`.