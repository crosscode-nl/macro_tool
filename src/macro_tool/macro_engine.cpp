#include "macro_tool/macro_engine.h"


namespace crosscode::macro_tool {

    std::vector<token_type> macro_lexer(std::string_view text, char macro_delimiter, char macro_param_separator) {
        std::vector<token_type> tokens;
        auto start = begin(text);
        auto current = begin(text);
        bool in_macro = false;
        while (current != end(text)) {
            if (*current == macro_delimiter) {
                if (!in_macro) {
                    if (start < current) { // empty strings don't need to be added
                        tokens.emplace_back(string_token{text.substr(start - begin(text), current - start)});
                    }
                    start = current + 1;
                    in_macro = true;
                } else {
                    if (start <=
                        current) { // empty macros do need to be added, because they can mean an escape for %
                        tokens.emplace_back(macro_token{text.substr(start - begin(text), current - start),macro_param_separator});
                    }
                    start = current + 1;
                    in_macro = false;
                }
            }
            current++;
        }
        if (start < current) {
            if (!in_macro) { // only handle strings, because open macros are broken macros
                tokens.emplace_back(string_token{text.substr(start - begin(text), current - start)});
            }
        }
        return tokens;
    }

    macro_token::macro_token(std::string_view macro, char macro_param_separator) {
        auto separator_pos = macro.find(macro_param_separator);
        if (separator_pos != macro_token::npos) {
            macro_name = macro.substr(0, separator_pos);
            macro_param = macro.substr(separator_pos + 1);
        } else {
            macro_name = macro;
        }
    }

}