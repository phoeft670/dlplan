#ifndef DLPLAN_SRC_CORE_PARSER_EXPRESSIONS_FACTORY_H_
#define DLPLAN_SRC_CORE_PARSER_EXPRESSIONS_FACTORY_H_

#include <vector>

#include "types.h"


namespace dlplan::core {
class VocabularyInfo;
namespace parser {

class ExpressionFactory {
public:
    Expression_Ptr make_expression(
        const VocabularyInfo& vocabulary_info,
        const std::string& name,
        std::vector<Expression_Ptr> &&children);
};

}
}

#endif
