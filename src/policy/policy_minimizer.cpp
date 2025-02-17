#include "condition.h"
#include "effect.h"

#include "../utils/set_operators.h"

#include "../../include/dlplan/policy.h"


namespace dlplan::policy {

/**
 * Copies all objects to the given PolicyBuilder and returns newly constructed objects.
 */
template<typename T>
std::vector<T> copy_to_builder(
    const std::vector<T>& old_objects,
    PolicyBuilder& builder) {
    std::vector<T> new_objects;
    new_objects.reserve(old_objects.size());
    std::transform(
        old_objects.begin(),
        old_objects.end(),
        std::back_inserter(new_objects),
        [&builder](const auto& object){
            return object->copy_to_builder(builder);
        }
    );
    return new_objects;
}


/**
 * Returns true iff all objects of given type PARENT_T are of type SUB_T
 */
template<typename PARENT_T, typename SUB_T>
static bool check_subtype_equality(
    const std::vector<std::shared_ptr<const PARENT_T>>& objects) {
    return std::all_of(
        objects.begin(),
        objects.end(),
        [](const std::shared_ptr<const PARENT_T>& object){
            return std::dynamic_pointer_cast<const SUB_T>(object);
        }
    );
}


/**
 * Returns true iff all objects of given type T have feature with same index.
 */
template<typename T>
static bool check_feature_index_equality(
    const std::vector<std::shared_ptr<const T>>& objects) {
    if (objects.empty()) return true;
    return std::all_of(
        objects.begin(),
        objects.end(),
        [index=(*(objects.begin()))->get_base_feature()->get_index()](
            const std::shared_ptr<const T>& object){
                return object->get_base_feature()->get_index() == index;
            }
        );
}


static std::vector<std::shared_ptr<const Rule>> try_merge_by_condition(
    const Policy& policy, PolicyBuilder& builder) {
    for (const auto& rule_1 : policy.get_rules()) {
        for (const auto& rule_2 : policy.get_rules()) {
            if (rule_1->get_index() >= rule_2->get_index()) {
                continue;
            }
            if (rule_1->get_effects() != rule_2->get_effects()) {
                continue;
            }
            std::vector<std::shared_ptr<const BaseCondition>> symmetric_diff = utils::set_symmetric_difference<std::shared_ptr<const BaseCondition>>({rule_1->get_conditions(), rule_2->get_conditions()});
            if (symmetric_diff.empty()) {
                continue;
            }
            if (!check_feature_index_equality(symmetric_diff) ||
                !(check_subtype_equality<BaseCondition, BooleanCondition>(symmetric_diff) || check_subtype_equality<BaseCondition, NumericalCondition>(symmetric_diff))) {
                continue;
            }
            builder.add_rule(
                copy_to_builder(utils::set_difference(rule_1->get_conditions(), symmetric_diff), builder),
                copy_to_builder(rule_1->get_effects(), builder));
            return {rule_1, rule_2};
        }
    }
    return {};
}


static std::vector<std::shared_ptr<const Rule>> try_merge_by_effect(
    const Policy& policy, PolicyBuilder& builder) {
    for (const auto& rule_1 : policy.get_rules()) {
        for (const auto& rule_2 : policy.get_rules()) {
            if (rule_1->get_index() >= rule_2->get_index()) {
                continue;
            }
            if (rule_1->get_conditions() != rule_2->get_conditions()) {
                continue;
            }
            std::vector<std::shared_ptr<const BaseEffect>> symmetric_diff = utils::set_symmetric_difference<std::shared_ptr<const BaseEffect>>({rule_1->get_effects(), rule_2->get_effects()});
            if (symmetric_diff.empty()) {
                continue;
            }
            if (!check_feature_index_equality(symmetric_diff) ||
                !(check_subtype_equality<BaseEffect, BooleanEffect>(symmetric_diff) ||
                  check_subtype_equality<BaseEffect, NumericalEffect>(symmetric_diff))) {
                continue;
            }
            for (const auto& rule_3 : policy.get_rules()) {
                if (rule_2->get_index() >= rule_3->get_index()) {
                    continue;
                }
                if (rule_2->get_conditions() != rule_3->get_conditions()) {
                    continue;
                }
                symmetric_diff = utils::set_symmetric_difference<std::shared_ptr<const BaseEffect>>({rule_1->get_effects(), rule_2->get_effects(), rule_3->get_effects()});
                if (symmetric_diff.empty()) {
                    continue;
                }
                if (!check_feature_index_equality(symmetric_diff) ||
                    !(check_subtype_equality<BaseEffect, BooleanEffect>(symmetric_diff) ||
                      check_subtype_equality<BaseEffect, NumericalEffect>(symmetric_diff))) {
                    continue;
                }
                builder.add_rule(
                    copy_to_builder(rule_1->get_conditions(), builder),
                    copy_to_builder(utils::set_difference(rule_1->get_effects(), symmetric_diff), builder));
                return {rule_1, rule_2, rule_3};
            }
        }
    }
    return {};
}


static std::vector<std::shared_ptr<const Rule>> compute_dominated_rules(
    const Policy& policy) {
    std::unordered_set<std::shared_ptr<const Rule>> dominated_rules;
    for (const auto& rule_1 : policy.get_rules()) {
        for (const auto& rule_2 : policy.get_rules()) {
            if (rule_1 == rule_2) {
                // Note: there cannot be identical rules in a policy, hence this equality check suffices to not remove all identical rules.
                continue;
            }
            if (utils::is_subset_eq(rule_1->get_conditions(), rule_2->get_conditions()) && utils::is_subset_eq(rule_1->get_effects(), rule_2->get_effects())) {
                dominated_rules.insert(rule_2);
                break;
            }
        }
    }
    return std::vector<std::shared_ptr<const Rule>>(dominated_rules.begin(), dominated_rules.end());
}


/**
 * Returns true iff policy classifies true_state_pairs as true and false_state_pairs as false.
 */
static bool check_policy_matches_classification(
    const Policy& policy,
    const core::StatePairs& true_state_pairs,
    const core::StatePairs& false_state_pairs) {
    return std::all_of(true_state_pairs.begin(), true_state_pairs.end(), [&policy](const core::StatePair& state_pair){ return policy.evaluate_lazy(state_pair.first, state_pair.second); }) &&
           std::all_of(false_state_pairs.begin(), false_state_pairs.end(), [&policy](const core::StatePair& state_pair){ return !policy.evaluate_lazy(state_pair.first, state_pair.second); });
}


PolicyMinimizer::PolicyMinimizer() { }

PolicyMinimizer::PolicyMinimizer(const PolicyMinimizer& other) = default;

PolicyMinimizer& PolicyMinimizer::operator=(const PolicyMinimizer& other) = default;

PolicyMinimizer::PolicyMinimizer(PolicyMinimizer&& other) = default;

PolicyMinimizer& PolicyMinimizer::operator=(PolicyMinimizer&& other) = default;

PolicyMinimizer::~PolicyMinimizer() { }

Policy PolicyMinimizer::minimize(const Policy& policy) const {
    // Merge rules
    // TODO: avoid rechecking merges.
    Policy current_policy = policy;
    std::vector<std::shared_ptr<const Rule>> merged_rules;
    do {
        PolicyBuilder builder;
        merged_rules = try_merge_by_condition(current_policy, builder);
        if (merged_rules.empty()) {
            merged_rules = try_merge_by_effect(current_policy, builder);
        }
        copy_to_builder(utils::set_difference(current_policy.get_rules(), merged_rules), builder);
        current_policy = builder.get_result();
    } while (!merged_rules.empty());
    // Remove dominated rules
    PolicyBuilder builder;
    copy_to_builder(utils::set_difference(current_policy.get_rules(), compute_dominated_rules(current_policy)), builder);
    current_policy = builder.get_result();
    return current_policy;
}

Policy PolicyMinimizer::minimize(const Policy& policy, const core::StatePairs& true_state_pairs, const core::StatePairs& false_state_pairs) const {
    // TODO: avoid rechecking conditions
    Policy current_policy = policy;
    bool minimization_success;
    do {
        minimization_success = false;
        for (const auto& rule : current_policy.get_rules()) {
            for (const auto& condition : rule->get_conditions()) {
                PolicyBuilder builder;
                builder.add_rule(
                    copy_to_builder(utils::set_difference(rule->get_conditions(), {condition}), builder),
                    copy_to_builder(rule->get_effects(), builder));
                copy_to_builder(utils::set_difference(current_policy.get_rules(), {rule}), builder);
                Policy tmp_policy = builder.get_result();
                if (check_policy_matches_classification(tmp_policy, true_state_pairs, false_state_pairs)) {
                    minimization_success = true;
                    current_policy = tmp_policy;
                    break;
                }
            }
            if (minimization_success) {
                break;
            }
            for (const auto& effect : rule->get_effects()) {
                PolicyBuilder builder;
                builder.add_rule(
                    copy_to_builder(rule->get_conditions(), builder),
                    copy_to_builder(utils::set_difference(rule->get_effects(), {effect}), builder));
                copy_to_builder(utils::set_difference(current_policy.get_rules(), {rule}), builder);
                Policy tmp_policy = builder.get_result();
                if (check_policy_matches_classification(tmp_policy, true_state_pairs, false_state_pairs)) {
                    minimization_success = true;
                    current_policy = tmp_policy;
                    break;
                }
            }
            if (minimization_success) {
                break;
            }
        }
    } while (minimization_success);
    return current_policy;
}

}