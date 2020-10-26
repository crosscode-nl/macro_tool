#ifndef MACRO_TOOL_MACRO_RENDER_ENGINE_H
#define MACRO_TOOL_MACRO_RENDER_ENGINE_H

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

    struct macro_name_token : public std::string {
        using std::string::string;
    };

    struct macro_param_token : public std::string {
        using std::string::string;
    };

    using token_type = std::variant<string_token, macro_name_token, macro_param_token>;

    std::vector<token_type> macro_lexer(std::string_view text, char macro_delimiter, char macro_param_separator);

    template <typename Tmacro_handler>
    class macro_render_engine {
    public:
        using macro_handler_type = Tmacro_handler;
    private:
        std::vector<token_type> tokens_;
        macro_handler_type macro_handler_;
    public:

        explicit macro_render_engine(std::vector<token_type> tokens) : tokens_{std::move(tokens)} {}

        std::string render() {
            std::string result;

            auto iterator = begin(tokens_);
            while (iterator!=end(tokens_)) {
                if (std::holds_alternative<string_token>(*iterator)) {
                    result += std::get<string_token>(*iterator);
                    iterator++;
                } else if (std::holds_alternative<macro_name_token>(*iterator)) {
                    std::string_view macro_name = std::get<macro_name_token>(*iterator);
                    std::string_view macro_param;
                    auto param_iterator = iterator+1;
                    if (param_iterator!=end(tokens_)&&std::holds_alternative<macro_param_token>(*param_iterator)) {
                        macro_param = std::get<macro_param_token>(*param_iterator);
                        iterator++;
                    }
                    result += macro_handler_.handle(macro_name,macro_param);
                    iterator++;
                } else { // invalid token, skipping.
                    iterator++;
                }
            }

            return result;
        }
    };

    template<typename Tmacro_engine, char macro_delimiter='%', char macro_param_separator=':'>
    std::string render_macros(std::string_view text) {
        return macro_render_engine<Tmacro_engine>{macro_lexer(text,macro_delimiter,macro_param_separator)}.render();
    }
}

#endif //MACRO_TOOL_MACRO_RENDER_ENGINE_H
