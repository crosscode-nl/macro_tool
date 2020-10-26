#include "doctest.h"
#include "macro_tool.h"

namespace cmt = crosscode::macro_tool;

using namespace std::literals;

struct macro_handler {
    static std::string handle_empty() {
        return "EMPTY";
    }

    static std::string handle_test(std::string_view param) {
        std::string result{"THIS"};
        if (!param.empty()) {
            result += "("s + std::string{param} + ")"s;
        }
        return result;
    }

    static std::string handle(std::string_view macro, std::string_view param) {
        if (macro.empty()) {
            return handle_empty();
        }
        if (macro=="TEST") {
            return handle_test(param);
        }
        return std::string{};
    }
};

TEST_SUITE("Macro tool tests") {
    TEST_CASE("Can render empty string") {
        REQUIRE(""==cmt::render_macros<macro_handler>(""));
    }
    TEST_CASE("Can render unknown macro string") {
        REQUIRE(""==cmt::render_macros<macro_handler>("%UNKNOWN%"));
    }
    TEST_CASE("Can render macro with param separator") {
        REQUIRE("THIS"==cmt::render_macros<macro_handler>("%TEST:%"));
    }
    TEST_CASE("Can render open macro with param separator") {
        REQUIRE("PREFIX THIS"==cmt::render_macros<macro_handler>("PREFIX %TEST:%%OPEN"));
    }
    TEST_CASE("render_macros can render macro") {
        REQUIRE("Test THISEMPTY macro"==cmt::render_macros<macro_handler>("Test %TEST%%% macro"));
    }
    TEST_CASE("render_macros can render macro with param with delimiter and delimiter $ and separator |") {
        REQUIRE("Test THIS(MY)EMPTY macro"==cmt::render_macros<macro_handler,'$','|'>("Test $TEST|MY$$$ macro"));
    }
}
