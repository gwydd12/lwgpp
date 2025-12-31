#ifndef LWGPP_GOTOPARSER_H
#define LWGPP_GOTOPARSER_H
#include <map>
#include <unordered_set>
#include "Parser.h"

class GOTOParser final : public Parser {
public:
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
    void checkGotoValues();
    static void fillStatementsWithNops(std::vector<std::unique_ptr<Statement>> &statements, int firstLine, int lastLine);
};


#endif //LWGPP_GOTOPARSER_H