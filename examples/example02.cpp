#include <iostream>
#include "macro_tool.h"
#include <charconv>

// This advanced example demonstrates a macro_handler with state and a constructor, and the use of render_engine.
struct stateful_macro_handler {

    int64_t counter_;

    explicit stateful_macro_handler(int64_t counter) : counter_{counter} {}

    std::string handle(std::string_view macro, std::string_view param) {
        if (macro=="COUNTER") {
            counter_++;
            if (!param.empty()) {
                int min_digits;
                auto[p, ec] = std::from_chars(std::data(param), std::data(param) + std::size(param), min_digits);
                if (ec == std::errc()) {
                    auto result = std::to_string(counter_);
                    min_digits-=std::size(result);
                    if (min_digits>0) {
                        result = std::string(min_digits,'0') + result;
                    }
                    return result;
                }
            }
            return std::to_string(counter_);
        }
        return std::string{};
    }
};

int main() {
    // makes all symbols in crosscode::macro_tool available in the current scope
    using namespace crosscode::macro_tool;

    // A call to render_macros<echo>() with the provided text will result in a text that has all macros
    // replaces with content provided by the echo macro handler. This time we also have to provide
    // parameters of for the constructor of stateful_macro_handler
    std::cout << "Using render_macros:\n";
    std::cout << render_macros<stateful_macro_handler>("%COUNTER% and another %COUNTER:4%", 0) << "\n";
    std::cout << render_macros<stateful_macro_handler>("%COUNTER% and another %COUNTER:4%", 0) << "\n\n";

    // However, if you want to call it again later, because you need the same text updated with newer values
    // you better use the render_engine. This time you need to call the lexer yourself.
    std::cout << "Using render_engine:\n";
    macro_render_engine<stateful_macro_handler> render_engine{macro_lexer("%COUNTER% and another %COUNTER:4%"),0};

    // Now you can call render multiple times and state will be preserved.
    std::cout << render_engine.render() << "\n";
    std::cout << render_engine.render() << "\n";
    return 0;
}
// The output will be:
// Using render_macros:
// 1 and another 0002
// 1 and another 0002
//
// Using render_engine:
// 1 and another 0002
// 3 and another 0004
