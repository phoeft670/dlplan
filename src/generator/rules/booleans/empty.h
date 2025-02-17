#ifndef DLPLAN_SRC_GENERATOR_RULES_BOOLEANS_EMPTY_H_
#define DLPLAN_SRC_GENERATOR_RULES_BOOLEANS_EMPTY_H_

#include "../rule.h"
#include "../../../core/elements/booleans/empty.h"


namespace dlplan::generator::rules {

class EmptyBoolean : public Rule {
public:
    EmptyBoolean() : Rule() { }

    void generate_impl(const States& states, int target_complexity, GeneratorData& data, core::DenotationsCaches& caches) override {
        core::SyntacticElementFactory& factory = data.m_factory;
        for (const auto& concept : data.m_concepts_by_iteration[target_complexity-1]) {
            auto element = factory.make_empty_boolean(concept);
            auto denotations = element.get_element_ref().evaluate(states, caches);
            if (data.m_boolean_and_numerical_hash_table.insert(denotations).second) {
                data.m_reprs.push_back(element.compute_repr());
                data.m_booleans_by_iteration[target_complexity].push_back(std::move(element));
                increment_generated();
            }
        }
        for (const auto& role : data.m_roles_by_iteration[target_complexity-1]) {
            auto element = factory.make_empty_boolean(role);
            auto denotations = element.get_element_ref().evaluate(states, caches);
            if (data.m_boolean_and_numerical_hash_table.insert(denotations).second) {
                data.m_reprs.push_back(element.compute_repr());
                data.m_booleans_by_iteration[target_complexity].push_back(std::move(element));
                increment_generated();
            }
        }
    }

    std::string get_name() const override {
        return core::element::EmptyBoolean<int>::get_name();
    }
};

}

#endif
