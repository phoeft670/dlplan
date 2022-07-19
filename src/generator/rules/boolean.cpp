#include "boolean.h"


namespace dlplan::generator::rules {

std::function<BooleanTaskResult(const States&, const core::Boolean&)> Boolean::m_task =
[](const States& states, const core::Boolean& element) {
    return BooleanTaskResult(
        core::Boolean(element),
        element.compute_repr(),
        compute_hash(evaluate_boolean(element, states)));
};

void Boolean::parse_results_of_tasks_impl(int iteration, GeneratorData& data) {
    /* Wait for the result and add it. */
    while (!m_tasks.empty()) {
        if (data.reached_resource_limit()) return;
        auto result = m_tasks.front().get();
        m_tasks.pop_front();
        ++data.m_num_generated_features;
        if (data.m_boolean_and_numerical_hash_table.insert(std::move(result.hash)).second) {
            ++data.m_num_novel_features;
            ++m_count;
            data.m_reprs.push_back(std::move(result.repr));
            data.m_booleans_by_iteration[iteration+1].push_back(std::move(result.boolean));
        }
    }
}

void Boolean::cleanup() {
    m_tasks.clear();
}

}
