#ifndef MACRO_TOOL_MACRO_ENGINE_H
#define MACRO_TOOL_MACRO_ENGINE_H

#include <variant>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
#include <map>

namespace crosscode::macro_tool {

    /// Quick and dirty macro tool.
    /// lexes an input string into strings and macros
    /// On a render call each handler registered with register_macro is called to generate a string to replace
    /// the macro with.
    class macro_engine {
    private:
        struct string_token : public std::string {
            using std::string::string;
        };

        struct macro_token : public std::string {
            explicit macro_token(std::string_view macro, char macro_param_separator);

            std::string macro_name;
            std::string macro_param;
        };

        friend class token_visitor;
    public:
        using macro_handler_type = std::function<std::string(std::string_view param)>;
        using macro_handlers_type = std::map<std::string, macro_handler_type, std::less<>>;
        using token_type = std::variant<string_token, macro_token>;
    private:
        std::vector<token_type> items_;
        macro_handlers_type macro_handlers_;
    public:
        explicit macro_engine(std::string_view text, char macro_delimiter='%', char macro_param_separator=':');
        void register_macro_handler(std::string macro, macro_handler_type f);
        std::string render();
    };
}

#endif //MACRO_TOOL_MACRO_ENGINE_H
