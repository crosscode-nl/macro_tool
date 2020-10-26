#ifndef MACRO_TOOL_MACRO_ENGINE_H
#define MACRO_TOOL_MACRO_ENGINE_H

#include <variant>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
#include <map>

namespace crosscode::macro_tool {

    struct string_token : public std::string {
        using std::string::string;
    };

    struct macro_token : public std::string {
        explicit macro_token(std::string_view macro, char macro_param_separator);

        std::string macro_name;
        std::string macro_param;
    };

    using token_type = std::variant<string_token, macro_token>;

    template <typename Tmacro_handler>
    class token_visitor {
    public:
        using macro_handler_type = Tmacro_handler;
    private:
        macro_handler_type macro_handler_;
    public:
        std::string operator()(const string_token &st) const {
            return st;
        }

        std::string operator()(const macro_token &mt) const {
            return macro_handler_.handle(mt.macro_name, mt.macro_param);
        }
    };

    std::vector<token_type> macro_lexer(std::string_view text, char macro_delimiter, char macro_param_separator);

    /// Quick and dirty macro tool.
    /// lexes an input string into strings and macros
    /// On a render call each handler registered with register_macro is called to generate a string to replace
    /// the macro with.
    template <typename Tmacro_handler>
    class macro_engine {
    public:
        using token_visitor_type = token_visitor<Tmacro_handler>;
    private:
        std::vector<token_type> tokens_{};
        const token_visitor_type token_visitor_{};
    public:
        explicit macro_engine(std::string_view text, char macro_delimiter='%', char macro_param_separator=':')
        : tokens_{macro_lexer(text,macro_delimiter,macro_param_separator)} {}
        std::string render() {
            auto string_concat = [this](const std::string &s, token_type tt) {
                return s + std::visit(token_visitor_, tt);
            };
            return std::accumulate(begin(tokens_), end(tokens_), std::string{}, string_concat);
        }
    };

    template<typename Tmacro_engine, char macro_delimiter='%', char macro_param_separator=':'>
    std::string render_macros(std::string_view text) {
        return macro_engine<Tmacro_engine>{text,macro_delimiter,macro_param_separator}.render();
    }
}

#endif //MACRO_TOOL_MACRO_ENGINE_H
