#include "macro_tool/macro_engine.h"


namespace crosscode::macro_tool {

    macro_engine::macro_token::macro_token(std::string_view macro, char macro_param_separator) {
        auto separator_pos = macro.find(macro_param_separator);
        if (separator_pos != macro_token::npos) {
            macro_name = macro.substr(0, separator_pos);
            macro_param = macro.substr(separator_pos + 1);
        } else {
            macro_name = macro;
        }
    }

    macro_engine::macro_engine(std::string_view text, char macro_delimiter, char macro_param_separator) {
        auto start = begin(text);
        auto current = begin(text);
        bool inMacro = false;
        while (current != end(text)) {
            if (*current == macro_delimiter) {
                if (!inMacro) {
                    if (start < current) { // empty strings don't need to be added
                        items_.emplace_back(string_token{text.substr(start - begin(text), current - start)});
                    }
                    start = current + 1;
                    inMacro = true;
                } else {
                    if (start <=
                        current) { // empty macros do need to be added, because they can mean an escape for %
                        items_.emplace_back(macro_token{text.substr(start - begin(text), current - start),macro_param_separator});
                    }
                    start = current + 1;
                    inMacro = false;
                }
            }
            current++;
        }
        if (start < current) {
            if (!inMacro) { // only handle strings, because open macros are broken macros
                items_.emplace_back(string_token{text.substr(start - begin(text), current - start)});
            }
        }
    }

    void macro_engine::register_macro_handler(std::string macro, macro_handler_type f) {
        macro_handlers_.emplace(macro_handlers_type::value_type{std::move(macro), std::move(f)});
    }

    class token_visitor {
    public:
        using macro_handlers_type = macro_engine::macro_handlers_type;
        using macro_token = macro_engine::macro_token;
        using string_token = macro_engine::string_token;
    private:
        const macro_handlers_type &macro_handlers_;
    public:
        explicit token_visitor(const macro_handlers_type &macro_handlers) : macro_handlers_{macro_handlers} {}

        std::string operator()(const string_token &st) const {
            return st;
        }

        std::string operator()(const macro_token &mt) const {
            auto handler = macro_handlers_.find(mt.macro_name);
            if (handler != end(macro_handlers_)) {
                return handler->second(mt.macro_param);
            }
            return std::string{};
        }
    };

    std::string macro_engine::render() {
        auto string_concat = [token_visitor = token_visitor{macro_handlers_}](const std::string &s, token_type tt) {
            return s + std::visit(token_visitor, tt);
        };
        return std::accumulate(begin(items_), end(items_), std::string{}, string_concat);
    }

}