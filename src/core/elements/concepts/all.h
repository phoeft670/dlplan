#ifndef DLPLAN_SRC_CORE_ELEMENTS_CONCEPTS_ALL_H_
#define DLPLAN_SRC_CORE_ELEMENTS_CONCEPTS_ALL_H_

#include "../concept.h"
#include "../role.h"


namespace dlplan::core::element {

class AllConcept : public Concept {
protected:
    const Role_Ptr m_role;
    const Concept_Ptr m_concept;

public:
    AllConcept(const VocabularyInfo& vocabulary, Role_Ptr role, Concept_Ptr concept)
    : Concept(vocabulary, "c_all"), m_role(role), m_concept(concept) {
        if (!(role && concept)) {
            throw std::runtime_error("AllConcept::AllConcept - at least one child is a nullptr");
        }
    }

    ConceptDenotation evaluate(const State& state) const override {
        const auto role_denot = m_role->evaluate(state);
        const auto concept_denot = m_concept->evaluate(state);
        ConceptDenotation result = state.get_instance_info()->get_top_concept();
        // find counterexamples b : exists b . (a,b) in R and b notin C
        for (const auto& pair : role_denot) {
            if (!concept_denot.contains(pair.second)) {
                result.erase(pair.first);
            }
        }
        return result;
    }

    ConceptDenotation evaluate(EvaluationContext& context) const override {
        // check cache
        auto find = context.concept_cache.find(get_index(), context.state);
        if (find != context.concept_cache.end()) {
            return find->second;
        }
        // Create new cache entry and compute the result inplace.
        auto result = context.concept_cache.insert(std::make_pair(get_index(), context.state->get_instance_info()->get_top_concept()));
        auto& result_denotation = result.first->second;
        // Actual computation: find counterexamples b : exists b . (a,b) in R and b notin C
        const auto role_denot = m_role->evaluate(context);
        const auto concept_denot = m_concept->evaluate(context);
        for (const auto& pair : role_denot) {
            if (!concept_denot.contains(pair.second)) {
                result_denotation.erase(pair.first);
            }
        }
        // return result stored in cache;
        return result.first->second;
    }

    int compute_complexity() const override {
        return m_role->compute_complexity() + m_concept->compute_complexity() + 1;
    }

    void compute_repr(std::stringstream& out) const override {
        out << m_name << "(";
        m_role->compute_repr(out);
        out << ",";
        m_concept->compute_repr(out);
        out << ")";
    }
};

}

#endif
