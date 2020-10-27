#include <benchmark/benchmark.h>
#include <sstream>
#include "macro_tool.h"

using namespace crosscode::macro_tool;

struct macro_handler {
    static void begin_render(){};
    static void done_render(){};

    static std::string handle(std::string_view,std::string_view) {
        return {};
    }
};

std::string create_string_10_bytes_text_24_bytes_macro_x_factor(int factor) {
    std::string result;
    result.reserve(44*factor+1);
    for (int counter=0; counter<factor; counter++) {
        result+="12345%MACRO%67890%MACRO:PARAMETER%";
    }
    return result;
}

static void BM_RenderMacros(benchmark::State& state) {

    std::string macro_text = create_string_10_bytes_text_24_bytes_macro_x_factor(state.range());
    for (auto _ : state) {
        benchmark::DoNotOptimize(render_macros<macro_handler>(macro_text));
    }
}
BENCHMARK(BM_RenderMacros)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_MacroEngine(benchmark::State& state) {

    std::string macro_text = create_string_10_bytes_text_24_bytes_macro_x_factor(state.range());
    macro_render_engine<macro_handler> macro_engine{macro_lexer(macro_text)};
    for (auto _ : state) {
        benchmark::DoNotOptimize(macro_engine.render());
    }
}

BENCHMARK(BM_MacroEngine)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_MacroEngineWithReserve(benchmark::State& state) {

    std::string macro_text = create_string_10_bytes_text_24_bytes_macro_x_factor(state.range());
    macro_render_engine<macro_handler> macro_engine{macro_lexer(macro_text)};
    for (auto _ : state) {
        benchmark::DoNotOptimize(macro_engine.render(10*state.range()));
    }
}

BENCHMARK(BM_MacroEngineWithReserve)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();