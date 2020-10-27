#include <iostream>
#include "macro_tool.h"

// This example demonstrates the most simple usage of this library.
//
// A macro handler must be provided.
// A macro handler should be a class implementing a function called handle with the following signatures:
// std::string handle(std::string_view macro, std::string_view param)
// void begin_render(){};
// void done_render(){};
// or
// static std::string handle(std::string_view macro, std::string_view param)
// static void begin_render(){};
// static void done_render(){};
// This demonstrates a simple macro handler that formats the macro %NAME% as NAME and %NAME|PARAM% as NAME(PARAM)
struct echo {
    static void begin_render(){};
    static void done_render(){};

    static std::string handle(std::string_view macro, std::string_view param) {
        if (param.empty()) return std::string{macro};
        return std::string{macro} + "(" + std::string{param} + ")";
    }
};

int main() {
    // makes all symbols in crosscode::macro_tool available in the current scope
    using namespace crosscode::macro_tool;

    // A call to render_macros<echo>() with the provided text will result in a text that has all macros
    // replaces with content provided by the echo macro handler.
    // By default macros are pair delimited with % characters and inside the macro a param is separated from
    // the macro name with a : character.
    std::cout << render_macros<echo>("%COUNTER% and another %COUNTER:4%");

    return 0;
}
// The output will be:
// COUNTER and another COUNTER(4)
