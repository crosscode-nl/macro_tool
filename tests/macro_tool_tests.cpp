#include "doctest.h"
#include "macro_tool.h"

namespace cmt = crosscode::macro_tool;

using namespace std::literals;

struct macro_handler {
    int counter_{};

    void begin_render() {
        counter_++;
    }

    void done_render() {
        counter_+=2;
    }

    [[nodiscard]]
    std::string handle_counter() const {
        return std::to_string(counter_);
    }

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

    [[nodiscard]]
    std::string handle(std::string_view macro, std::string_view param) const {
        if (macro.empty()) {
            return handle_empty();
        }
        if (macro=="TEST") {
            return handle_test(param);
        }
        if (macro=="COUNTER") {
            return handle_counter();
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
    TEST_CASE("begin_render and done_render are called") {
        cmt::macro_render_engine<macro_handler> m{cmt::macro_lexer("%COUNTER%")};
        REQUIRE(m.render()=="1");
        REQUIRE(m.render()=="4");
    }
    TEST_CASE("render_macros can render macro") {
        REQUIRE("Test THISEMPTY macro"==cmt::render_macros<macro_handler>("Test %TEST%%% macro"));
    }
    TEST_CASE("render_macros can render macro with param with delimiter and delimiter $ and separator |") {
        REQUIRE("Test THIS(MY)EMPTY macro"==cmt::render_macros<macro_handler,'$','|'>("Test $TEST|MY$$$ macro"));
    }
}
