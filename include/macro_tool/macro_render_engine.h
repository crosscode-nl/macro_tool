#ifndef MACRO_TOOL_MACRO_RENDER_ENGINE_H
#define MACRO_TOOL_MACRO_RENDER_ENGINE_H

#include <variant>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
#include <map>

/// This crosscode::macro_tool namespace contains all code provided by this package.
namespace crosscode::macro_tool {

    /// string_token is a token containing a string
    struct string_token : public std::string {
        using std::string::string;
    };

    /// \brief macro_name_token is a token containing the name of a macro
    ///
    /// A macro_name_token must be succeeded by one macro_param_token
    /// The reason a macro name and macro param are not in a single token is to make sure
    /// the token_type variant is using memory most optimal.
    struct macro_name_token : public std::string {
        using std::string::string;
    };

    /// \brief macro_param_token is a token containing the parameter of a macro
    ///
    /// A macro_param_token must be preceded by one macro_param_token
    /// The reason a macro name and macro param are not in a single token is to make sure
    /// the token_type variant is using memory most optimal.
    struct macro_param_token : public std::string {
        using std::string::string;
    };

    /// \brief token_variant is a variant that can hold a string_token, macro_name_token or a macro_param_token
    using token_variant = std::variant<string_token, macro_name_token, macro_param_token>;

    /// \brief tokens_vector is a vector containing variants with tokens.
    using token_variant_vector = std::vector<token_variant>;

    /// \brief macro_lexer tokenizes a text containing macros.
    ///
    /// \param text The text with macros to perform the lexical analysis on
    /// \param macro_delimiter The delimiter of the macros.
    /// \param macro_param_separator The separator for a macro parameter.
    /// \return tokens is returned, its a vector containing the tokens of type token_type, which is a variant.
    token_variant_vector macro_lexer(std::string_view text, char macro_delimiter, char macro_param_separator);

    /// \brief macro_render_engine renders macro tokens into a resulting text.
    /// \tparam Tmacro_handler is the type of the macro_handler.
    template <typename Tmacro_handler>
    class macro_render_engine {
    public:
        /// macro_handler_type is the type of the macro_handler
        using macro_handler_type = Tmacro_handler;
    private:
        /// tokens_ contains the tokens
        token_variant_vector tokens_;
        /// macro_handler_ contains an instance of the macro_handler which will provide texts for each macro+param
        macro_handler_type macro_handler_;
    public:
        /// macro_render_engine constructor creates an instance of a macro_render_engine
        /// \tparam Args A parameter pack of arguments for Tmacro_handler
        /// \param tokens The tokenized representation of the macro holding text.
        /// \param args The arguments for the constructor of Tmacro_handler.
        template <typename ...Args>
        explicit macro_render_engine(token_variant_vector tokens, Args... args)
            : tokens_{std::move(tokens)},
              macro_handler_{std::forward<Args>(args)...} {}

        /// render takes the tokens and turns this into a resulting text
        ///
        /// string_token is directly concatenated to the output string
        /// for each macro_name_token and macro_param_token a call to macro_handler_.handle(macro_name,macro_param)
        /// is made. The result of this call is a string which is then concatenated to the output string.
        /// \return The rendered text.
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

    /// render_macros is a helper function for situations where the macro holding text is only rendered once.
    ///
    /// When you need to render the text multiple times, for example in case of timestamps,
    /// then use macro_render_engine because that saves time lexing the macro holding text each time.
    /// \tparam Tmacro_handler The type of the macro_handler to use.
    /// \tparam macro_delimiter The delimiter of the macros.
    /// \tparam macro_param_separator The separator for a macro parameter.
    /// \tparam Args A parameter pack of arguments for Tmacro_handler
    /// \param text The text with macros to perform the lexical analysis on
    /// \param args The arguments for the constructor of Tmacro_handler.
    /// \return The rendered text.
    template<typename Tmacro_handler, char macro_delimiter='%', char macro_param_separator=':', typename ...Args>
    std::string render_macros(std::string_view text, Args ...args) {
        return macro_render_engine<Tmacro_handler>{macro_lexer(text,macro_delimiter,macro_param_separator),std::forward<Args>(args)...}.render();
    }
}

#endif //MACRO_TOOL_MACRO_RENDER_ENGINE_H
