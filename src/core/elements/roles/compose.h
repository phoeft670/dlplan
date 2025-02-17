#ifndef DLPLAN_SRC_CORE_ELEMENTS_ROLES_COMPOSE_H_
#define DLPLAN_SRC_CORE_ELEMENTS_ROLES_COMPOSE_H_

#include "../role.h"
#include "../utils.h"


namespace dlplan::core::element {

class ComposeRole : public Role {
private:
    void compute_result(const RoleDenotation& left_denot, const RoleDenotation& right_denot, int num_objects, RoleDenotation& result) const {
        const auto left_bitset = element::utils::role_denot_to_bitset(left_denot);
        const auto right_bitset = element::utils::role_denot_to_bitset(right_denot);
        for (int i = 0; i < num_objects; ++i) {  // source
            for (int j = 0; j < num_objects; ++j) {  // target
                for (int k = 0; k < num_objects; ++k) {  // middle
                    if (left_bitset.test(i * num_objects + k) && right_bitset.test(k * num_objects + j)) {
                        result.insert(std::make_pair(i, j));
                        break;
                    }
                }
            }
        }
    }

    std::unique_ptr<RoleDenotation> evaluate_impl(const State& state, DenotationsCaches& caches) const override {
        auto denotation = std::make_unique<RoleDenotation>(
            RoleDenotation(state.get_instance_info_ref().get_num_objects()));
        compute_result(
            *m_role_left->evaluate(state, caches),
            *m_role_right->evaluate(state, caches),
            state.get_instance_info_ref().get_num_objects(),
            *denotation);
        return denotation;
    }

    std::unique_ptr<RoleDenotations> evaluate_impl(const States& states, DenotationsCaches& caches) const override {
        auto denotations = std::make_unique<RoleDenotations>();
        denotations->reserve(states.size());
        auto role_left_denotations = m_role_left->evaluate(states, caches);
        auto role_right_denotations = m_role_right->evaluate(states, caches);
        for (size_t i = 0; i < states.size(); ++i) {
            auto denotation = std::make_unique<RoleDenotation>(
                RoleDenotation(states[i].get_instance_info_ref().get_num_objects()));
            compute_result(
                *(*role_left_denotations)[i],
                *(*role_right_denotations)[i],
                states[i].get_instance_info_ref().get_num_objects(),
                *denotation);
            denotations->push_back(caches.m_r_denot_cache.insert(std::move(denotation)).first->get());
        }
        return denotations;
    }

protected:
    const Role_Ptr m_role_left;
    const Role_Ptr m_role_right;

public:
    ComposeRole(const VocabularyInfo& vocabulary, Role_Ptr role_left, Role_Ptr role_right)
    : Role(vocabulary), m_role_left(role_left), m_role_right(role_right)  {
        if (!(role_left && role_right)) {
            throw std::runtime_error("ComposeRole::ComposeRole - at least one child is a nullptr.");
        }
    }

    RoleDenotation evaluate(const State& state) const override {
        int num_objects = state.get_instance_info_ref().get_num_objects();
        RoleDenotation denotation(num_objects);
        compute_result(
            m_role_left->evaluate(state),
            m_role_right->evaluate(state),
            num_objects,
            denotation);
        return denotation;
    }

    int compute_complexity() const override {
        return m_role_left->compute_complexity() + m_role_right->compute_complexity() + 1;
    }

    void compute_repr(std::stringstream& out) const override {
        out << get_name() << "(";
        m_role_left->compute_repr(out);
        out << ",";
        m_role_right->compute_repr(out);
        out << ")";
    }

    static std::string get_name() {
        return "r_compose";
    }
};

}

#endif
