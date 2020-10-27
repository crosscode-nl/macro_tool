# macro_tool
[![GitHub version](https://badge.fury.io/gh/crosscode-nl%2Fmacro_tool.svg)](https://badge.fury.io/gh/crosscode-nl%2Fmacro_tool)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/crosscode-nl/macro_tool.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/crosscode-nl/macro_tool/context:cpp)
[![Build Status](https://travis-ci.com/crosscode-nl/macro_tool.svg?branch=main)](https://travis-ci.com/crosscode-nl/macro_tool) 
[![Build status](https://ci.appveyor.com/api/projects/status/uw3t7p76iqqjklpw?svg=true)](https://ci.appveyor.com/project/crosscode-nl/macro-tool)
[![Standard](https://img.shields.io/badge/c%2B%2B-17/20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Introduction

macro_tool is a lean and simple C++17 library that helps parsing simple text macros in the following form:

"This text is created in year %DATE:yyyy% and written by %AUTHOR%"

Nested macros are not supported, and it also not meant as an advanced template engine.

You can change the single character macro pair delimiter and single character macro parameter separator. 

Using this library saves you writing code lexing/tokenizing this text, and you now only have write the code that 
replaces the macros. 

```cpp
#include <iostream>
#include <chrono>
#include "macro_tool.h"

// You implement your macro handler.
struct macro_handler {
    std::chrono::system_clock::time_point time_point_;

    void begin_render() {
        time_point_ = std::chrono::system_clock::now();
    }

    static void done_render() {}

    static std::string handle_reverse(std::string_view param) {
        std::string result;
        // Optional: Reserve size. Saves allocations if you know the size already and SSO is not possible.
        result.reserve(size(param));  
        std::copy(rbegin(param),rend(param),back_inserter(result));
        return result;
    }

    [[nodiscard]]
    std::string handle_date(std::string_view param) const {
        auto time = std::chrono::system_clock::to_time_t(time_point_);
        auto tm = std::localtime(&time);

        if (param=="YEAR") return std::to_string(tm->tm_year+1900);

        std::string_view date_string = {std::asctime(tm)};
        date_string.remove_suffix(1);
        return std::string{date_string};
    }

    [[nodiscard]]
    std::string handle(std::string_view macro, std::string_view param) const {
        if (macro=="REVERSE") return handle_reverse(param);
        if (macro=="AUTHOR") return "CrossCode";
        if (macro=="DATE") return handle_date(param);
        return std::string{};
    }
};

// You could create a _macro literal if you have lots of texts with macros in your code.
std::string operator ""_macro(const char* text, std::size_t size) {
    using namespace crosscode::macro_tool;
    // You use the render_macros with your handler.
    return render_macros<macro_handler>(std::string_view{text,size});
}

// Now enjoy your macro replacements. 
int main() {
    std::cout << "(c)%DATE:YEAR% %AUTHOR%\nThis macro replacement was executed on: %DATE%\n"_macro;
    std::cout << "I want to write backwards backwards, which is: %REVERSE:backwards%\n"_macro;
    return 0;
}
// (c)2020 CrossCode
// This macro replacement was executed on: Tue Oct 27 15:55:04 2020
// I want to write backwards backwards, which is: sdrawkcab
```

See the examples for a deeper explanation of the library. 

## Installation

There are multiple ways to add this library to your project. There are too many tools for C++ to describe them all. 

I will describe two methods, both for CMake users. 

The first method will download the source code and build against it, the second will use a system installed version of 
the library.

The first method works best in CI/CD pipelines without to much hassle. This is therefore the preferred method.

### CMake FetchContent (Preferred)

This method emulates dependency management of more modern languages and build systems like Rust+Cargo and Go as best as 
it can.

Consider the following example project: 

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
```

This will build a C++17 project with a main.cpp file.

Now to add this library to the project you can use the 
[FetchContent CMake module](https://cmake.org/cmake/help/v3.18/module/FetchContent.html). 

The minimal requirement is to add: 

```cmake
include(FetchContent)
FetchContent_Declare(
        macro_tool
        GIT_REPOSITORY https://github.com/crosscode-nl/macro_tool
        GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(macro_tool)
target_link_libraries(${PROJECT_NAME} macro_tool)
```

This will result in the following CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

include(FetchContent)
FetchContent_Declare(
        macro_tool
        GIT_REPOSITORY https://github.com/crosscode-nl/macro_tool
        GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(macro_tool)
target_link_libraries(${PROJECT_NAME} macro_tool)
```

You can also put the first block in a `cmake/macro_tool.cmake` file and include it as following: 

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

include(cmake/macro_tool.cmake)
```
 
Now see the examples to learn how to use this library.

*TIP: Use `GIT_TAG` in `FetchContent_Declare` to pin a certain version to get reproducible builds.*

### CMake Find_Package

This method will use CMake's find_package.

The first step is to install the library on the system.

#### Installing onto the system

This method requires macro_tool to be build and installed on the system.

```bash
git clone https://github.com/crosscode-nl/macro_tool
mkdir macro_tool-build
cd macro_tool-build
cmake ../macro_tool -DCMAKE_BUILD_TYPE=Release
sudo make install 
```

Or if you want to install it somewhere in you home directory, for example: 

```bash
git clone https://github.com/crosscode-nl/macro_tool
mkdir macro_tool-build
cd macro_tool-build
cmake ../macro_tool -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/home/developer/libraries
make install 
```

#### Using the installed library

Consider the following example project: 

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
```

To use an installed library you just have to add: 

```cmake
find_package(MacroTool 1.2.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} MacroTool::macro_tool)
```

This will result in the following CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.17)
project(example)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_package(MacroTool 1.2.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} MacroTool::macro_tool)
```

When you installed the library in a different location then you have to add 
`-DCMAKE_PREFIX_PATH=/home/developer/libraries` to your 
[CMake command](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html). 

## Examples

Examine the following examples to learn how to use this library.

* [example01.cpp](examples/example01.cpp) This example demonstrates the most simple usage of this library.
* [example02.cpp](examples/example02.cpp) This advanced example demonstrates a macro_handler with state and a constructor, and the use of render_engine.
* [example03.cpp](examples/example03.cpp) This example demonstrates non default macro delimiters and macro param separators.

## License

MIT License

Copyright (c) 2020 CrossCode / P. Vollebregt

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the 
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the 
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
