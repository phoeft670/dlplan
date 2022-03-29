#include "../../include/dlplan/core.h"

#include "elements/concept.h"


namespace dlplan::core {

PerElementEvaluationCache::PerElementEvaluationCache() = default;

PerElementEvaluationCache::PerElementEvaluationCache(const PerElementEvaluationCache& other) = default;

PerElementEvaluationCache& PerElementEvaluationCache::operator=(const PerElementEvaluationCache& other) = default;

PerElementEvaluationCache::PerElementEvaluationCache(PerElementEvaluationCache&& other) = default;

PerElementEvaluationCache& PerElementEvaluationCache::operator=(PerElementEvaluationCache&& other) = default;

PerElementEvaluationCache::~PerElementEvaluationCache() = default;

void PerElementEvaluationCache::clear_cache() {
    m_concept_idx_to_denotation.clear();
    m_role_idx_to_denotation.clear();
}

const ConceptDenotation& PerElementEvaluationCache::retrieve_or_evaluate(const element::Concept& concept, EvaluationContext& evaluation_context) {
    // check cache validity.
    if (m_cached_state != evaluation_context.state) {
        m_cached_state = evaluation_context.state;
        clear_cache();
    }
    // check cache miss
    auto find = m_concept_idx_to_denotation.find(concept.get_index());
    if (find == m_concept_idx_to_denotation.end()) {
        m_concept_idx_to_denotation.emplace(concept.get_index(), concept.evaluate(*evaluation_context.state));
    }
    return m_concept_idx_to_denotation.at(concept.get_index());
}

const RoleDenotation& PerElementEvaluationCache::retrieve_or_evaluate(const element::Role& role, EvaluationContext& evaluation_context) {

}

}