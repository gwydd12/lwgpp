#ifndef LWGPP_GOTOPARSER_H
#define LWGPP_GOTOPARSER_H
#include <map>
#include <unordered_set>
#include "Parser.h"

namespace goto_parser {
    class GotoParser final : public parser::Parser {
    public:
        GotoParser() = default;
        /**
         * Move constructor for GotoParser.
         * Transfers ownership of resources from another GotoParser instance.
         * Actually must not be explicitly defined because the compiler would generate one automatically, due to
         * the fact that all member variables have move constructors.
         * Just for demonstration purposes.
         * @param other GotoParser rvalue reference to another GotoParser instance
         */
        GotoParser(GotoParser&& other) noexcept
            : markerNumbers_(std::move(other.markerNumbers_)),
              gotoValuesMap_(std::move(other.gotoValuesMap_)),
              markerLineMap_(std::move(other.markerLineMap_)),
              containsHalt_(other.containsHalt_)
        {
            other.containsHalt_ = false;
        }

        // Same for move assignment operator
        GotoParser& operator=(GotoParser&& other) noexcept {
            if (this != &other) {
                markerNumbers_ = std::move(other.markerNumbers_);
                gotoValuesMap_ = std::move(other.gotoValuesMap_);
                markerLineMap_ = std::move(other.markerLineMap_);
                containsHalt_ = other.containsHalt_;
                other.containsHalt_ = false;
            }
            return *this;
        }

        std::vector<std::unique_ptr<Statement>>
        parse(std::vector<Token> tokens) override;
        std::map<std::string, int> getMarkerLineMap();

    private:
        std::unordered_set<int> markerNumbers_;
        std::map<std::string, int> gotoValuesMap_;
        std::map<std::string, int> markerLineMap_;
        bool containsHalt_ = false;

        std::vector<std::unique_ptr<Statement>> parseGoto();
        std::unique_ptr<Halt> parseHalt(int line, int markerLine);
        std::unique_ptr<If> parseIf(int line, int markerLine);
        std::unique_ptr<Goto> parseGotoStatement(int line, int markerLine);
        void validateGotoMarkerValues();
        static void fillStatementsWithNops(std::vector<std::unique_ptr<Statement>> &statements, int firstLine, int lastLine);
    };
}

#endif //LWGPP_GOTOPARSER_H