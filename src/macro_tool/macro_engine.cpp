#include "macro_tool/macro_engine.h"


namespace crosscode::macro_tool {

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