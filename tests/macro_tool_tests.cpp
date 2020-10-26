#include "doctest.h"
#include "macro_tool.h"

namespace cmt = crosscode::macro_tool;

TEST_SUITE("Macro tool tests") {
    TEST_CASE("macro_engine can render macro") {
        cmt::macro_engine macro{"Test %TEST%%% macro"};
        REQUIRE("Test  macro"==macro.render());
        SUBCASE("%TEST% macro replaced with THIS") {
            macro.register_macro_handler("",[](std::string_view) {
                return "%";
            });
            macro.register_macro_handler("TEST",[](std::string_view) {
                return "THIS";
            });
            REQUIRE("Test THIS% macro"==macro.render());
        }
    }
    TEST_CASE("macro_engine can render macro with param") {
        cmt::macro_engine macro{"Test %TEST:MY%%% macro"};
        REQUIRE("Test  macro"==macro.render());
        SUBCASE("%TEST:MY% macro replaced with THIS(MY)") {
            macro.register_macro_handler("TEST",[](std::string_view p) {
                return std::string{"THIS("} + std::string{p} + ")";
            });
            REQUIRE("Test THIS(MY) macro"==macro.render());
        }
    }
    TEST_CASE("macro_engine can render macro with param with delimiter $ and separator |") {
        cmt::macro_engine macro{"Test $TEST|MY$$$ macro",'$','|'};
            REQUIRE("Test  macro"==macro.render());
            SUBCASE("$TEST|MY$ macro replaced with THIS(MY)") {
            macro.register_macro_handler("TEST",[](std::string_view p) {
                return std::string{"THIS("} + std::string{p} + ")";
            });
            REQUIRE("Test THIS(MY) macro"==macro.render());
        }
    }
}
