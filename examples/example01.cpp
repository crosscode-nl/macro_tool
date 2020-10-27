#include <iostream>
#include "macro_tool.h"

// This example demonstrates the most simple usage of this library
//
// A macro handler must be provided.
// A macro handler should be a class implementing a function called handle with the following signature:
// std::string handle(std::string_view macro, std::string_view param)
// or
// static std::string handle(std::string_view macro, std::string_view param)
//
// This demonstrates a simple macro handler that formats the macro %EXAMPLE% as EXAMPLE and %EXAMPLE|PARAMETER%
// as EXAMPLE(PARAMETER)
struct echo {
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
    std::cout <<
              render_macros<echo>(
                      "This macro contains an %EXAMPLE% macro and %EXAMPLE|10% macro with parameter"
              );

    return 0;
}
// The output will be:
// This macro contains an EXAMPLE macro and EXAMPLE|10 macro with parameter
