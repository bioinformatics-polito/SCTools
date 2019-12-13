# SCTools - Single-Cell utilities tools

**SCTools** is a suite of tools performing utility operations over
single-cell samples.

## Configure, build and install
**SCTools** is versioned using *git* and can be retrieved by typing
the following command on the command line:
```
git clone https://github.com/bioinformatics-polito/SCTools.git
```

**SCTools** is written in *C++* and configured via *CMake*. For being
configured, it requires at least *CMake* version 3.7.2. Once the
project is downloaded from the remote *git* repository, the 
configuration process is triggered by
```
cd SCTools
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
If the build process is successful, the executables can be 
retrieved from the `apps` directory within the build sub-tree.

## Examples
The **SCTools** repository comes with example scripts providing real-world
use-cases for demonstrating the capabilities of the suite. All examples
are executable scripts under the `examples` directory:
- `01_demultiplex_alignment_file.sh` provides an example of single-cell
BAM file de-multiplexing according to a list of cell barcodes.

