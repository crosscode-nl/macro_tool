#include "doctest.h"
#include "macro_tool.h"

namespace cmt = crosscode::macro_tool;

using namespace std::literals;

struct macro_handler {
    std::string handle(std::string_view macro, std::string_view param) const {
        if (macro.empty()) {
            return "%";
        }
        if (macro=="TEST") {
            std::string result{"THIS"};
            if (!param.empty()) {
                result += "("s + std::string{param} + ")"s;
            }
            return result;
        }
        return std::string{};
    }
};

TEST_SUITE("Macro tool tests") {
    TEST_CASE("macro_engine can render macro") {
        cmt::macro_engine<macro_handler> macro{"Test %TEST%%% macro"};
        REQUIRE("Test THIS% macro"==macro.render());
    }
    TEST_CASE("render_macros can render macro") {
        REQUIRE("Test THIS% macro"==cmt::render_macros<macro_handler>("Test %TEST%%% macro"));
    }
    TEST_CASE("macro_engine can render macro with param") {
        cmt::macro_engine<macro_handler> macro{"Test %TEST:MY%%% macro"};
        REQUIRE("Test THIS(MY)% macro"==macro.render());
    }
    TEST_CASE("macro_engine can render macro with param with delimiter $ and separator |") {
        cmt::macro_engine<macro_handler> macro{"Test $TEST|MY$$$ macro",'$','|'};
        REQUIRE("Test THIS(MY)% macro"==macro.render());
    }
    TEST_CASE("render_macros can render macro with param with delimiter $ and separator |") {
        REQUIRE("Test THIS(MY)% macro"==cmt::render_macros<macro_handler,'$','|'>("Test $TEST|MY$$$ macro"));
    }
}
