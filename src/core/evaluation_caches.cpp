#include "evaluation_caches.h"

#include "elements/concept.h"
#include "elements/role.h"


namespace dlplan::core {

EvaluationCachesImpl::EvaluationCachesImpl(std::shared_ptr<const InstanceInfo> instance_info)
   : m_instance_info(instance_info),
     m_num_objects(instance_info->get_num_objects()),
     m_concept_denotation_cache(std::vector<bool>(m_num_objects, false)),
     m_role_denotation_cache(std::vector<bool>(m_num_objects * m_num_objects, false)) {
}

ConceptDenotation EvaluationCachesImpl::try_retrieve_or_evaluate(EvaluationCaches* parent, const State& state, const element::Concept& concept) {
    int concept_index = concept.get_index();
    auto insert_result = m_concept_index_to_cache_index.emplace(concept_index, m_concept_index_to_cache_index.size());
    int cache_index = insert_result.first->second;
    bool cache_status = insert_result.second;
    ConceptDenotation result(m_num_objects, m_concept_denotation_cache[cache_index]);
    if (!cache_status) {
        concept.evaluate(state, *parent, result);
    }
    return result;
}

RoleDenotation EvaluationCachesImpl::try_retrieve_or_evaluate(EvaluationCaches* parent, const State& state, const element::Role& role) {
    int role_index = role.get_index();
    auto insert_result = m_role_index_to_cache_index.emplace(role_index, m_role_index_to_cache_index.size());
    int cache_index = insert_result.first->second;
    bool cache_status = insert_result.second;
    RoleDenotation result(m_num_objects, m_role_denotation_cache[cache_index]);
    if (!cache_status) {
        role.evaluate(state, *parent, result);
    }
    return result;
}

void EvaluationCachesImpl::clear() {
    m_concept_index_to_cache_index.clear();
    m_role_index_to_cache_index.clear();
}

std::shared_ptr<const InstanceInfo> EvaluationCachesImpl::get_instance_info() const {
    return m_instance_info;
}

}