#ifndef DLPLAN_SRC_GENERATOR_RULES_ROLES_PRIMITIVE_H_
#define DLPLAN_SRC_GENERATOR_RULES_ROLES_PRIMITIVE_H_

#include "../rule.h"
#include "../../../core/elements/roles/primitive.h"


namespace dlplan::generator::rules {

class PrimitiveRole : public Rule {
public:
    PrimitiveRole() : Rule() { }

    void generate_impl(const States& states, int target_complexity, GeneratorData& data, core::DenotationsCaches& caches) override {
        assert(target_complexity == 1);
        core::SyntacticElementFactory& factory = data.m_factory;
        for (const auto& predicate : factory.get_vocabulary_info_ref().get_predicates_ref()) {
            for (int arg1 = 0; arg1 < predicate.get_arity(); ++arg1) {
                for (int arg2 = arg1+1; arg2 < predicate.get_arity(); ++arg2) {
                    auto element = factory.make_primitive_role(predicate, arg1, arg2);
                    auto denotations = element.get_element_ref().evaluate(states, caches);
                    if (data.m_role_hash_table.insert(denotations).second) {
                        data.m_reprs.push_back(element.compute_repr());
                        data.m_roles_by_iteration[target_complexity].push_back(std::move(element));
                        increment_generated();
                    }
                }
            }
        }
    }

    std::string get_name() const override {
        return core::element::PrimitiveRole::get_name();
    }
};

}

#endif
