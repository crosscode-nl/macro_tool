#include "macro_tool/macro_render_engine.h"


namespace crosscode::macro_tool {

    token_variant_vector macro_lexer(std::string_view text, char macro_delimiter, char macro_param_separator) {
        token_variant_vector tokens;
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
                    if (start <= current) { // empty macros do need to be added, because they can mean an escape for %
                        auto macro_name_and_param = text.substr(start - begin(text), current - start);
                        auto macro_param_separator_pos = macro_name_and_param.find(macro_param_separator);
                        if (macro_param_separator_pos != decltype(macro_name_and_param)::npos) {
                            tokens.emplace_back(macro_name_token{macro_name_and_param.substr(0, macro_param_separator_pos)});
                            tokens.emplace_back(macro_param_token{macro_name_and_param.substr(macro_param_separator_pos + 1)});
                        } else {
                            tokens.emplace_back(macro_name_token{macro_name_and_param});
                        }
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


}