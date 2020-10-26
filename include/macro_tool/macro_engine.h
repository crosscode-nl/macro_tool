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

    /// Quick and dirty macro tool.
    /// lexes an input string into strings and macros
    /// On a render call each handler registered with register_macro is called to generate a string to replace
    /// the macro with.
    template <typename Tmacro_handler>
    class macro_engine {
    public:
        using token_visitor_type = token_visitor<Tmacro_handler>;
        using token_type = std::variant<string_token, macro_token>;
    private:
        std::vector<token_type> items_{};
        const token_visitor_type token_visitor_{};
    public:
        explicit macro_engine(std::string_view text, char macro_delimiter='%', char macro_param_separator=':') {
            auto start = begin(text);
            auto current = begin(text);
            bool in_macro = false;
            while (current != end(text)) {
                if (*current == macro_delimiter) {
                    if (!in_macro) {
                        if (start < current) { // empty strings don't need to be added
                            items_.emplace_back(string_token{text.substr(start - begin(text), current - start)});
                        }
                        start = current + 1;
                        in_macro = true;
                    } else {
                        if (start <=
                            current) { // empty macros do need to be added, because they can mean an escape for %
                            items_.emplace_back(macro_token{text.substr(start - begin(text), current - start),macro_param_separator});
                        }
                        start = current + 1;
                        in_macro = false;
                    }
                }
                current++;
            }
            if (start < current) {
                if (!in_macro) { // only handle strings, because open macros are broken macros
                    items_.emplace_back(string_token{text.substr(start - begin(text), current - start)});
                }
            }
        }
        std::string render() {
            auto string_concat = [this](const std::string &s, token_type tt) {
                return s + std::visit(token_visitor_, tt);
            };
            return std::accumulate(begin(items_), end(items_), std::string{}, string_concat);
        }
    };

    template<typename Tmacro_engine, char macro_delimiter='%', char macro_param_separator=':'>
    std::string render_macros(std::string_view text) {
        return macro_engine<Tmacro_engine>{text,macro_delimiter,macro_param_separator}.render();
    }
}

#endif //MACRO_TOOL_MACRO_ENGINE_H
