#ifndef LWGPP_LWPARSER_H
#define LWGPP_LWPARSER_H
#include "Parser.h"

namespace lw_parser {
    class LwParser final : public parser::Parser {
    public:
        std::vector<std::unique_ptr<Statement>>
        parse(std::vector<Token> tokens) override;

    private:
        std::deque<Token> balancedIteration;
        bool encounteredEnd = false;

        std::vector<std::unique_ptr<Statement>> parseLW();
        std::unique_ptr<Loop> parseLoop();
        std::unique_ptr<While> parseWhile();
        void parseEnd();
        void validateClosingSequence(int line) const;

        /**
         * SFINAE - Substitution Failure Is Not An Error
         * We use SFINAE to enable this function only for enum types.
         * This ensures that TokenCategory is an enum type at compile time.
         * Better approach: Using concepts to constrain the template parameter.
         */
        template<typename TokenCategory, TokenCategory... Expected,
                 std::enable_if_t<std::is_enum_v<TokenCategory>, int> = 0>
        bool isBalanced();
    };
}
#endif //LWGPP_LWPARSER_H