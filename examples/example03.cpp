#include <iostream>
#include "macro_tool.h"
#include <charconv>

// This example demonstrates non default macro delimiters and macro param separators.
//
// We will show how to change macro delimiter % into $ and how to change the macro param separator : into |.
struct echo {
    static std::string handle(std::string_view macro, std::string_view param) {
        if (param.empty()) return std::string{macro};
        return std::string{macro} + "(" + std::string{param} + ")";
    }
};

int main() {
    // makes all symbols in crosscode::macro_tool available in the current scope
    using namespace crosscode::macro_tool;

    // Replacing the delimiter and separator in render_macros is done via template values. This one having
    // to provide the default values for the delimiter and separator when the macro handler requires constructor
    // parameters.
    std::cout << "Using render_macros:\n";
    std::cout << render_macros<echo,'$','|'>("$COUNTER$ and another $COUNTER|4$") << "\n";

    // Replacing the delimiter and separator when using the render_engine is done via the macro_lexer by passing the values
    // as parameters.
    std::cout << "Using render_engine:\n";
    macro_render_engine<echo> render_engine{macro_lexer("$COUNTER$ and another $COUNTER|4$",'$','|')};

    // Now you can call render multiple times and state will be preserved.
    std::cout << render_engine.render() << "\n";
    return 0;
}
// The output will be:
// Using render_macros:
// COUNTER and another COUNTER(4)
// Using render_engine:
// COUNTER and another COUNTER(4)