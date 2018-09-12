# clippson

[![Build Status](https://travis-ci.org/heavywatal/clippson.svg?branch=master)](https://travis-ci.org/heavywatal/clippson)

This is a helper library of [muellan/clipp](https://github.com/muellan/clipp), the awesome C++ command-line parser.

- Support using [nlohmann/json](https://github.com/nlohmann/json) as a `boost::program_options::variables_map`-like object, which makes it possible to print all the variables at once to console/files in a computer-readable format.
- Create a pair of `clipp::option` and `clipp::value` (as a `clipp::group`) with a doc string including its default value.
- Apply a `clipp::match::` filter to each `clipp::value` automatically by detecting the type of its target variable.


## Installation

The easiest way is to use [Homebrew](https://brew.sh/)/[Linuxbrew](http://linuxbrew.sh/).
The following command installs tumopp and all the dependencies:
```sh
brew install heavywatal/tap/clippson
```

Alternatively, you can get the source code from GitHub manually:
```sh
git clone https://github.com/heavywatal/clippson.git
cd clippson/
git submodule update --init --recursive
mkdir build
cd build/
DESTINATION=${HOME}/local
cmake -DCMAKE_INSTALL_PREFIX=$DESTINATION ..
make -j2 install
```

`clippson.hpp` and the required headers, `clipp.h` and `json.hpp`, are installed to `${CMAKE_INSTALL_PREFIX}/include/clippson/`.
This library can be used with `find_package()` from other CMake projects.


## Usage

Add `#include <clippson/clippson.hpp>` to your source code.
Functions are defined in the namespace `wtl`.
The required headers `clipp.h` and `json.hpp` are included therein, hence their functions are available in the original namespaces.

See [test/example.cpp](https://github.com/heavywatal/clippson/blob/master/test/example.cpp) for more details.