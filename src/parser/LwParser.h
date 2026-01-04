#ifndef LWGPP_LWPARSER_H
#define LWGPP_LWPARSER_H
#include "Parser.h"

namespace parser::lw {
    class LwParser final : public Parser {
    public:
        std::vector<std::unique_ptr<Statement>>
        parse(std::vector<Token> tokens) override;

    private:
        std::deque<Token> balancedIteration_;
        bool encounteredEnd_ = false;

        /**
         * SFINAE - Substitution Failure Is Not An Error
         * We use SFINAE to enable this function only for enum types.
         * This ensures that TokenCategory is an enum type at compile time.
         * Better approach: Using concepts to constrain the template parameter.
         */
        template<typename TokenCategory, TokenCategory... Expected,
                 std::enable_if_t<std::is_enum_v<TokenCategory>, int> = 0>
        bool isBalanced() {
            if (balancedIteration_.empty()) return false;

            const Token& token = balancedIteration_.back();
            if ((... || token.is<TokenCategory, Expected>())) {
                balancedIteration_.pop_back();
                return true;
            }
            return false;
        }

        std::vector<std::unique_ptr<Statement>> parseLW();
        std::unique_ptr<Loop> parseLoop();
        std::unique_ptr<While> parseWhile();
        void parseEnd();
        void validateClosingSequence(int line) const;
    };
}
#endif //LWGPP_LWPARSER_H