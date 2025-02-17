#ifndef DLPLAN_SRC_CORE_ELEMENTS_ROLE_H_
#define DLPLAN_SRC_CORE_ELEMENTS_ROLE_H_

#include "element.h"


namespace dlplan::core::element {

class Role : public Element<RoleDenotation> {
protected:
    virtual std::unique_ptr<RoleDenotation> evaluate_impl(const State& state, DenotationsCaches& caches) const = 0;
    virtual std::unique_ptr<RoleDenotations> evaluate_impl(const States& states, DenotationsCaches& caches) const = 0;

public:
    explicit Role(const VocabularyInfo& vocabulary) : Element<RoleDenotation>(vocabulary) { }
    ~Role() override = default;

    /**
     * Evaluate without caching for a single state.
     */
    virtual RoleDenotation evaluate(const State& state) const = 0;

    /**
     * Evaluate with caching for a single state.
     */
    RoleDenotation* evaluate(const State& state, DenotationsCaches& caches) const {
        // check if denotations is cached.
        std::array<int, 3> key({state.get_instance_info_ref().get_index(), state.get_index(), get_index()});
        auto cached = caches.m_r_denots_mapping_per_state.find(key);
        if (cached != caches.m_r_denots_mapping_per_state.end()) return cached->second;
        // compute denotation
        auto denotation = evaluate_impl(state, caches);
        // register denotation and append it to denotations.
        auto result_denotation = caches.m_r_denot_cache.insert(std::move(denotation)).first->get();
        caches.m_r_denots_mapping_per_state.emplace(key, result_denotation);
        return result_denotation;
    }

    /**
     * Evaluate with caching for a collection of states.
     */
    RoleDenotations* evaluate(const States& states, DenotationsCaches& caches) const {
        // check if denotations is cached.
        auto cached = caches.m_r_denots_mapping.find(get_index());
        if (cached != caches.m_r_denots_mapping.end()) return cached->second;
        // compute denotations
        auto denotations = evaluate_impl(states, caches);
        // register denotations and return it.
        auto result_denotations = caches.m_r_denots_cache.insert(std::move(denotations)).first->get();
        caches.m_r_denots_mapping.emplace(get_index(), result_denotations);
        return result_denotations;
    }
};

}

#endif
