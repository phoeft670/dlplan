#ifndef DLPLAN_INCLUDE_DLPLAN_POLICY_H_
#define DLPLAN_INCLUDE_DLPLAN_POLICY_H_

#include <unordered_set>
#include <vector>
#include <memory>
#include <iostream>
#include <string>

#include "core.h"
#include "pimpl.h"


namespace dlplan {
namespace core {
    template<typename T>
    class Element;
}
namespace policy {
class RuleImpl;
class PolicyImpl;
class PolicyReaderImpl;
class PolicyWriterImpl;


class PolicyRoot {
private:
    PolicyRoot();
    friend class PolicyImpl;

    // Cache the results of state evaluations here.
    // This allows hiding the cache to the outside.

public:
    ~PolicyRoot();
};

class State {
private:
    // The identifier in the cache.
    void* m_key;
    // The actual state data.
    const std::shared_ptr<const core::State> m_state;

public:
    State(void* key, std::shared_ptr<const core::State> state);
    ~State();

    void* get_key() const;
    std::shared_ptr<const core::State> get_state() const;
};

/**
 * A Feature is shared across all conditions and effects that use it.
 */
template<typename T>
class Feature {
private:
    const std::shared_ptr<const PolicyRoot> m_root;
    const int m_index;

protected:
    Feature(std::shared_ptr<const PolicyRoot> root, int index);

public:
    virtual ~Feature();

    virtual T evaluate(const State& state) const = 0;

    int get_index() const;

    std::string str() const;

    std::string compute_repr() const;

    std::shared_ptr<const PolicyRoot> get_root() const;
};

class BooleanFeature : public Feature<bool> {
private:
    const core::Boolean m_boolean;

private:
    BooleanFeature(std::shared_ptr<const PolicyRoot> root, int index, core::Boolean&& boolean)
      : Feature<bool>(root, index), m_boolean(std::move(boolean)) { }
    friend class PolicyImpl;

public:
    bool evaluate(const State& state) const override;
};

class NumericalFeature : public Feature<int> {
private:
    const core::Numerical m_numerical;

private:
    NumericalFeature(std::shared_ptr<const PolicyRoot> root, int index, core::Numerical&& numerical)
      : Feature<int>(root, index), m_numerical(std::move(numerical)) { }
    friend class PolicyImpl;

public:
    int evaluate(const State& state) const override;
};


/**
 * All different kinds of conditions.
 */
class BaseCondition {
private:
    const std::shared_ptr<const PolicyRoot> m_root;

protected:
    virtual std::unique_ptr<BaseCondition> clone_impl() const = 0;

    BaseCondition(std::shared_ptr<const PolicyRoot> root) : m_root(root) { }

public:
    virtual ~BaseCondition() = default;

    virtual bool evaluate(const State& state) const = 0;

    virtual std::string str() const = 0;

    virtual std::string compute_repr() const;

    virtual std::unique_ptr<BaseCondition> clone() const;

    std::shared_ptr<const PolicyRoot> get_root() const;
};


/**
 * All different kinds of effects.
 */
class BaseEffect {
private:
    const std::shared_ptr<const PolicyRoot> m_root;

protected:
    virtual std::unique_ptr<BaseEffect> clone_impl() const = 0;

    BaseEffect(std::shared_ptr<const PolicyRoot> root) : m_root(root) { }

public:
    virtual ~BaseEffect() = default;

    virtual bool evaluate(const State& source, const State& target) const = 0;

    virtual std::string str() const = 0;

    virtual std::string compute_repr() const;

    virtual std::unique_ptr<BaseEffect> clone() const;

    std::shared_ptr<const PolicyRoot> get_root() const;
};


/**
 * A rule over Boolean and numerical features has form C -> E
 * where C is set of feature conditions and E is set of feature effects
 */
class Rule {
private:
    pimpl<RuleImpl> m_pImpl;

private:
    explicit Rule(
        std::shared_ptr<const PolicyRoot> root,
        std::unordered_set<std::shared_ptr<const BaseCondition>>&& conditions,
        std::unordered_set<std::shared_ptr<const BaseEffect>>&& effects);
    friend class PolicyImpl;

public:
    Rule(const Rule& other);
    Rule& operator=(const Rule& other);
    ~Rule();

    bool evaluate_conditions(const State& source) const;
    bool evaluate_effects(const State& source, const State& target) const;

    std::string str() const;

    std::string compute_repr() const;

    std::shared_ptr<const PolicyRoot> get_root() const;
};

/**
 * A policy is a set of rules over Boolean and numerical features.
 */
class Policy {
private:
    pimpl<PolicyImpl> m_pImpl;

public:
    Policy();
    Policy(const Policy& other);
    Policy& operator=(const Policy& other);
    ~Policy();

    std::shared_ptr<BooleanFeature> add_boolean_feature(core::Boolean b);
    std::shared_ptr<NumericalFeature> add_numerical_feature(core::Numerical n);

    /**
     * Uniquely adds a condition (resp. effect) to the policy and returns it.
     */
    std::shared_ptr<const BaseCondition> add_b_pos_condition(std::shared_ptr<const BooleanFeature> b);
    std::shared_ptr<const BaseCondition> add_b_neg_condition(std::shared_ptr<const BooleanFeature> b);
    std::shared_ptr<const BaseCondition> add_n_gt_condition(std::shared_ptr<const NumericalFeature> n);
    std::shared_ptr<const BaseCondition> add_n_eq_condition(std::shared_ptr<const NumericalFeature> n);
    std::shared_ptr<const BaseEffect> add_b_pos_effect(std::shared_ptr<const BooleanFeature> b);
    std::shared_ptr<const BaseEffect> add_b_neg_effect(std::shared_ptr<const BooleanFeature> b);
    std::shared_ptr<const BaseEffect> add_b_bot_effect(std::shared_ptr<const BooleanFeature> b);
    std::shared_ptr<const BaseEffect> add_n_inc_effect(std::shared_ptr<const NumericalFeature> n);
    std::shared_ptr<const BaseEffect> add_n_dec_effect(std::shared_ptr<const NumericalFeature> n);
    std::shared_ptr<const BaseEffect> add_n_bot_effect(std::shared_ptr<const NumericalFeature> n);

    /**
     * Uniquely adds a rule to the policy and returns it.
     */
    std::shared_ptr<const Rule> add_rule(
        std::unordered_set<std::shared_ptr<const BaseCondition>>&& conditions,
        std::unordered_set<std::shared_ptr<const BaseEffect>>&& effects);

    /**
     * Lazily evaluate the state pair.
     * Optimized to compute fewest features of smallest runtime complexity.
     */
    bool evaluate_lazy(const State& source, const State& target);

    std::string str() const;

    std::shared_ptr<const PolicyRoot> get_root() const;
    std::vector<std::shared_ptr<BooleanFeature>> get_boolean_features() const;
    std::vector<std::shared_ptr<NumericalFeature>> get_numerical_features() const;
};


/**
 * PolicyReader for reading general policy from bytestream.
 */
class PolicyReader {
private:
    pimpl<PolicyReaderImpl> m_pImpl;

public:
    PolicyReader();
    ~PolicyReader();

    Policy read(const std::string& data) const;
};

/**
 * PolicyWriter for writing general policy to bytestream.
 */
class PolicyWriter {
    pimpl<PolicyWriterImpl> m_pImpl;

public:
    PolicyWriter();
    ~PolicyWriter();

    std::string write(const Policy& policy) const;
};

}
}

//#include "policy.tpp"

#endif