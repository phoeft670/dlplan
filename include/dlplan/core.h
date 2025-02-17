#ifndef DLPLAN_INCLUDE_DLPLAN_CORE_H_
#define DLPLAN_INCLUDE_DLPLAN_CORE_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "types.h"

#include "phmap/phmap.h"

#include "utils/pimpl.h"
#include "utils/dynamic_bitset.h"
#include "utils/cache.h"
#include "utils/hashing.h"

/**
 * Forward declarations and usings
 */
namespace dlplan::core {
    class SyntacticElementFactoryImpl;
    class InstanceInfoImpl;
    class VocabularyInfoImpl;
    class SyntacticElementFactory;
    class InstanceInfo;
    class VocabularyInfo;
    class State;
    class ConceptDenotation;
    class RoleDenotation;
    namespace element {
        template<typename T>
        class Element;
        class Concept;
        class Role;
        class Numerical;
        class Boolean;
    }

    using States = std::vector<State>;
    using StatesSet = std::unordered_set<State>;
    using StatePair = std::pair<State, State>;
    using StatePairs = std::vector<StatePair>;

    using ConceptDenotations = std::vector<ConceptDenotation*>;
    using RoleDenotations = std::vector<RoleDenotation*>;
    using BooleanDenotations = std::vector<bool>;
    using NumericalDenotations = std::vector<int>;
}


/**
 * Template specializations
 */
namespace std {
    template<> struct hash<dlplan::core::State> {
        size_t operator()(const dlplan::core::State& state) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::ConceptDenotation>> {
        size_t operator()(const unique_ptr<dlplan::core::ConceptDenotation>& denotation) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::RoleDenotation>> {
        size_t operator()(const unique_ptr<dlplan::core::RoleDenotation>& denotation) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::ConceptDenotations>> {
        size_t operator()(const unique_ptr<dlplan::core::ConceptDenotations>& denotations) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::RoleDenotations>> {
        size_t operator()(const unique_ptr<dlplan::core::RoleDenotations>& denotations) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::BooleanDenotations>> {
        size_t operator()(const unique_ptr<dlplan::core::BooleanDenotations>& denotations) const noexcept;
    };
    template<>
    struct hash<unique_ptr<dlplan::core::NumericalDenotations>> {
        size_t operator()(const unique_ptr<dlplan::core::NumericalDenotations>& denotations) const noexcept;
    };
    template<> struct hash<vector<unsigned>> {
        size_t operator()(const vector<unsigned>& data) const noexcept;
    };
    template<> struct hash<vector<int>> {
        size_t operator()(const vector<int>& data) const noexcept;
    };
    template<> struct hash<std::array<int, 3>> {
        size_t operator()(const std::array<int, 3>& data) const noexcept;
    };
}


namespace dlplan::core {

class ConceptDenotation {
private:
    int m_num_objects;
    utils::DynamicBitset<unsigned> m_data;

public:
    // Special iterator for bitset representing set of integers
    class const_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = utils::DynamicBitset<unsigned>;
            using const_reference   = const value_type&;

            const_iterator(const_reference data, int num_objects, bool end=false);

            bool operator!=(const const_iterator& other) const;
            bool operator==(const const_iterator& other) const;

            const int& operator*() const;
            // Postfix increment
            const_iterator operator++(int);
            // Prefix increment
            const_iterator& operator++();

        private:
            const_reference m_data;
            int m_num_objects;
            int m_index;

        private:
            void seek_next();
    };

    explicit ConceptDenotation(int num_objects);
    ConceptDenotation(const ConceptDenotation& other);
    ConceptDenotation& operator=(const ConceptDenotation& other);
    ConceptDenotation(ConceptDenotation&& other);
    ConceptDenotation& operator=(ConceptDenotation&& other);
    ~ConceptDenotation();

    bool operator==(const ConceptDenotation& other) const ;
    bool operator!=(const ConceptDenotation& other) const;

    ConceptDenotation& operator&=(const ConceptDenotation& other);
    ConceptDenotation& operator|=(const ConceptDenotation& other);
    ConceptDenotation& operator-=(const ConceptDenotation& other);
    ConceptDenotation& operator~();

    const_iterator begin() const;
    const_iterator end() const;

    bool contains(int value) const;
    void set();
    void insert(int value);
    void erase(int value);

    int size() const;
    int count() const;
    bool empty() const;
    bool intersects(const ConceptDenotation& other) const;
    bool is_subset_of(const ConceptDenotation& other) const;

    std::vector<int> to_sorted_vector() const;
    const utils::DynamicBitset<unsigned>& get_bitset_ref() const;

    std::size_t compute_hash() const;

    int get_num_objects() const;
};


class RoleDenotation {
private:
    int m_num_objects;
    utils::DynamicBitset<unsigned> m_data;

public:
    // Special iterator for bitset representing set of pairs of ints.
    class const_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = utils::DynamicBitset<unsigned>;
            using const_reference   = const value_type&;

            const_iterator(const_reference data, int num_objects, bool end=false);

            bool operator!=(const const_iterator& other) const;
            bool operator==(const const_iterator& other) const;

            const std::pair<int, int>& operator*() const;
            std::pair<int, int>* operator->();
            // Postfix increment
            const_iterator operator++(int);
            // Prefix increment
            const_iterator& operator++();

        private:
            const_reference m_data;
            int m_num_objects;
            std::pair<int, int> m_indices;

        private:
            void seek_next();
    };

    explicit RoleDenotation(int num_objects);
    RoleDenotation(const RoleDenotation& other);
    RoleDenotation& operator=(const RoleDenotation& other);
    RoleDenotation(RoleDenotation&& other);
    RoleDenotation& operator=(RoleDenotation&& other);
    ~RoleDenotation();

    bool operator==(const RoleDenotation& other) const ;
    bool operator!=(const RoleDenotation& other) const;

    RoleDenotation& operator&=(const RoleDenotation& other);
    RoleDenotation& operator|=(const RoleDenotation& other);
    RoleDenotation& operator-=(const RoleDenotation& other);
    RoleDenotation& operator~();

    const_iterator begin() const;
    const_iterator end() const;

    bool contains(const std::pair<int, int>& value) const;
    void set();
    void insert(const std::pair<int, int>& value);
    void erase(const std::pair<int, int>& value);

    int size() const;
    int count() const;
    bool empty() const;
    bool intersects(const RoleDenotation& other) const;
    bool is_subset_of(const RoleDenotation& other) const;

    std::vector<std::pair<int, int>> to_sorted_vector() const;
    const utils::DynamicBitset<unsigned>& get_bitset_ref() const;

    const std::vector<unsigned>& get_blocks() const;
    std::size_t compute_hash() const;

    int get_num_objects() const;
};


/**
 * Compares two std::unique_ptr<T>
 * by comparing objects T.
 */
template<typename T>
struct DerefEqual {
    bool operator()(const T& left, const T& right) const {
        return *left == *right;
    }
};

struct DenotationsCaches {
    // Cache for single denotations.
    std::unordered_set<std::unique_ptr<ConceptDenotation>, std::hash<std::unique_ptr<ConceptDenotation>>, DerefEqual<std::unique_ptr<ConceptDenotation>>> m_c_denot_cache;
    std::unordered_set<std::unique_ptr<RoleDenotation>, std::hash<std::unique_ptr<RoleDenotation>>, DerefEqual<std::unique_ptr<RoleDenotation>>> m_r_denot_cache;
    // Cache for collections of denotations.
    std::unordered_set<std::unique_ptr<BooleanDenotations>, std::hash<std::unique_ptr<BooleanDenotations>>, DerefEqual<std::unique_ptr<BooleanDenotations>>> m_b_denots_cache;
    std::unordered_set<std::unique_ptr<NumericalDenotations>, std::hash<std::unique_ptr<NumericalDenotations>>, DerefEqual<std::unique_ptr<NumericalDenotations>>> m_n_denots_cache;
    std::unordered_set<std::unique_ptr<ConceptDenotations>, std::hash<std::unique_ptr<ConceptDenotations>>, DerefEqual<std::unique_ptr<ConceptDenotations>>> m_c_denots_cache;
    std::unordered_set<std::unique_ptr<RoleDenotations>, std::hash<std::unique_ptr<RoleDenotations>>, DerefEqual<std::unique_ptr<RoleDenotations>>> m_r_denots_cache;
    // Mapping from element index to denotations.
    std::unordered_map<int, BooleanDenotations*> m_b_denots_mapping;
    std::unordered_map<int, NumericalDenotations*> m_n_denots_mapping;
    std::unordered_map<int, ConceptDenotations*> m_c_denots_mapping;
    std::unordered_map<int, RoleDenotations*> m_r_denots_mapping;
    // Mapping from instance, state, element index to denotations
    std::unordered_map<std::array<int, 3>, int> m_n_denots_mapping_per_state;
    std::unordered_map<std::array<int, 3>, bool> m_b_denots_mapping_per_state;
    std::unordered_map<std::array<int, 3>, ConceptDenotation*> m_c_denots_mapping_per_state;
    std::unordered_map<std::array<int, 3>, RoleDenotation*> m_r_denots_mapping_per_state;
};


class Constant {
private:
    std::string m_name;
    int m_index;

    Constant(const std::string& name, int index);
    friend class VocabularyInfo;

public:
    Constant() = delete;
    Constant(const Constant& other);
    Constant& operator=(const Constant& other);
    Constant(Constant&& other);
    Constant& operator=(Constant&& other);
    ~Constant();

    bool operator==(const Constant& other) const;
    bool operator!=(const Constant& other) const;

    int get_index() const;
    const std::string& get_name_ref() const;
};


/**
 * A Predicate belongs to a specific vocabulary of a planning domain.
 */
class Predicate {
private:
    std::string m_name;
    int m_index;
    int m_arity;

    Predicate(const std::string& name, int index, int arity);
    friend class VocabularyInfo;

public:
    Predicate(const Predicate& other);
    Predicate& operator=(const Predicate& other);
    Predicate(Predicate&& other);
    Predicate& operator=(Predicate&& other);
    ~Predicate();

    bool operator==(const Predicate& other) const;
    bool operator!=(const Predicate& other) const;

    /**
     * Getters.
     */
    int get_index() const;
    const std::string& get_name_ref() const;
    int get_arity() const;
};


/**
 * An Object belongs to a specific instance.
 */
class Object {
private:
    std::string m_name;
    int m_index;

    Object(const std::string& name, int index);
    friend class InstanceInfo;

public:
    Object(const Object& other);
    Object& operator=(const Object& other);
    Object(Object&& other);
    Object& operator=(Object&& other);
    ~Object();

    bool operator==(const Object& other) const;
    bool operator!=(const Object& other) const;

    int get_index() const;
    const std::string& get_name_ref() const;
};


/**
 * An Atom belongs to a specific instance.
 */
class Atom {
private:
    std::string m_name;
    int m_index;
    Predicate m_predicate;
    std::vector<Object> m_objects;
    bool m_is_static;

    Atom(const std::string& name,
        int index,
        const Predicate& predicate,
        const std::vector<Object> &objects,
        bool is_static);
    friend class InstanceInfo;

public:
    Atom(const Atom& other);
    Atom& operator=(const Atom& other);
    Atom(Atom&& other);
    Atom& operator=(Atom&& other);
    ~Atom();

    bool operator==(const Atom& other) const;
    bool operator!=(const Atom& other) const;

    /**
     * Getters.
     */
    const std::string& get_name_ref() const;
    int get_index() const;
    const Predicate& get_predicate_ref() const;
    const std::vector<Object>& get_objects_ref() const;
    const Object& get_object_ref(int pos) const;
    bool get_is_static() const;
};


/**
 * A State contains consists of atoms including all static atoms.
 * TODO: Think of switching to Bitset as underlying datastructure.
 * If there are small number of atoms this is more memory efficient.
 * per_predicate_idx_to_atom_idxs requires |A¦*|P| many bits.
 */
class State {
private:
    std::shared_ptr<const InstanceInfo> m_instance_info;
    Index_Vec m_atom_idxs;
    int m_index;

public:
    State(std::shared_ptr<const InstanceInfo> instance_info, const std::vector<Atom>& atoms, int index=-1);
    /**
     * Expert interface to construct states without the overhead
     * of copying Atoms but instead working on indices directly.
     */
    State(std::shared_ptr<const InstanceInfo> instance_info, const Index_Vec& atom_idxs, int index=-1);
    State(const State& other);
    State& operator=(const State& other);
    State(State&& other);
    State& operator=(State&& other);
    ~State();

    bool operator==(const State& other) const;
    bool operator!=(const State& other) const;

    /**
     * Computes string-like representation of the state.
     */
    std::string str() const;

    /**
     * Compute a 64-Bit hash value.
     */
    size_t compute_hash() const;

    /**
     * Setters.
     */
    void set_index(int index);

    /**
     * Getters.
     */
    const InstanceInfo& get_instance_info_ref() const;
    std::shared_ptr<const InstanceInfo> get_instance_info() const;
    const Index_Vec& get_atom_idxs_ref() const;
    Index_Vec compute_sorted_atom_idxs() const;
    int get_index() const;
};


/**
 * VocabularyInfo stores information related to the planning domain.
 */
class VocabularyInfo {
private:
    std::unordered_map<std::string, unsigned> m_predicate_name_to_predicate_idx;
    std::vector<Predicate> m_predicates;

    std::unordered_map<std::string, unsigned> m_constant_name_to_constant_idx;
    std::vector<Constant> m_constants;

public:
    VocabularyInfo();
    VocabularyInfo(const VocabularyInfo& other);
    VocabularyInfo& operator=(const VocabularyInfo& other);
    VocabularyInfo(VocabularyInfo&& other);
    VocabularyInfo& operator=(VocabularyInfo&& other);
    ~VocabularyInfo();

    const Predicate& add_predicate(const std::string &name, int arity);

    const Constant& add_constant(const std::string& name);

    bool exists_predicate(const Predicate& predicate) const;
    bool exists_predicate_name(const std::string& name) const;
    const std::vector<Predicate>& get_predicates_ref() const;
    int get_predicate_idx(const std::string& name) const;
    const Predicate& get_predicate_ref(int index) const;
    bool exists_constant(const Constant& constant) const;
    bool exists_constant_name(const std::string& name) const;
    int get_constant_idx(const std::string& name) const;
    const Constant& get_constant_ref(int index) const;
    const std::vector<Constant>& get_constants_ref() const;
};


/**
 * InstanceInfo stores information related to the planning instance.
 */
class InstanceInfo {
private:
    std::shared_ptr<const VocabularyInfo> m_vocabulary_info;
    int m_index;

    std::unordered_map<std::string, unsigned> m_atom_name_to_atom_idx;
    std::vector<Atom> m_atoms;

    std::unordered_map<std::string, unsigned> m_static_atom_name_to_static_atom_idx;
    std::vector<Atom> m_static_atoms;
    phmap::flat_hash_map<int, std::vector<int>> m_per_predicate_idx_static_atom_idxs;

    std::unordered_map<std::string, unsigned> m_object_name_to_object_idx;
    std::vector<Object> m_objects;

    const Atom& add_atom(const std::string &predicate_name, const Name_Vec &object_names, bool is_static);
    const Atom& add_atom(const Predicate& predicate, const std::vector<Object>& objects, bool is_static);

public:
    InstanceInfo() = delete;
    InstanceInfo(std::shared_ptr<const VocabularyInfo> vocabulary_info, int index=-1);
    InstanceInfo(const InstanceInfo& other);
    InstanceInfo& operator=(const InstanceInfo& other);
    InstanceInfo(InstanceInfo&& other);
    InstanceInfo& operator=(InstanceInfo&& other);
    ~InstanceInfo();

    /**
     * Alternative 1 to add atoms.
     */
    const Object& add_object(const std::string& object_name);
    const Atom& add_atom(const Predicate& predicate, const std::vector<Object>& objects);
    const Atom& add_static_atom(const Predicate& predicate, const std::vector<Object>& objects);

    /**
     * Alternative 2 to add atoms.
     */
    const Atom& add_atom(const std::string& predicate_name, const Name_Vec& object_names);
    const Atom& add_static_atom(const std::string& predicate_name, const Name_Vec& object_names);

    /**
     * Setters.
     */
    void set_index(int index);

    /**
     * Getters.
     */
    int get_index() const;
    bool exists_atom(const Atom& atom) const;
    const std::vector<Atom>& get_atoms_ref() const;
    const std::vector<Atom>& get_static_atoms_ref() const;
    const Atom& get_atom_ref(int index) const;
    int get_atom_idx(const std::string& name) const;
    bool exists_object(const Object& object) const;
    bool exists_object(const std::string name) const;
    const std::vector<Object>& get_objects_ref() const;
    const Object& get_object_ref(int index) const;
    int get_object_idx(const std::string& name) const;
    int get_num_objects() const;
    const VocabularyInfo& get_vocabulary_info_ref() const;
    std::shared_ptr<const VocabularyInfo> get_vocabulary_info() const;
    const phmap::flat_hash_map<int, std::vector<int>>& get_per_predicate_idx_static_atom_idxs_ref() const;
};


class BaseElement : public utils::Cachable {
protected:
protected:
    std::shared_ptr<const VocabularyInfo> m_vocabulary_info;
    /**
     * Index can be used for external caching.
     */
    int m_index;

protected:
    BaseElement(std::shared_ptr<const VocabularyInfo> vocabulary_info, int index=-1);

public:
    virtual ~BaseElement();

    /**
     * Returns the complexity of the element
     * measured in the size of the abstract syntax tree.
     */
    virtual int compute_complexity() const = 0;

    /**
     * Returns a canonical string representation.
     */
    virtual std::string compute_repr() const = 0;

    /**
     * Setters.
     */
    void set_index(int index);

    /**
     * Getters.
     */
    int get_index() const;
    const VocabularyInfo& get_vocabulary_info_ref() const;
    std::shared_ptr<const VocabularyInfo> get_vocabulary_info() const;
};


/**
 * Concept evaluates to ConceptDenotation.
 */
class Concept : public BaseElement {
private:
    std::shared_ptr<const element::Concept> m_element;

    Concept(std::shared_ptr<const VocabularyInfo> vocabulary_info, std::shared_ptr<const element::Concept>&& concept, int index=-1);
    friend class SyntacticElementFactoryImpl;

public:
    Concept(const Concept& other);
    Concept& operator=(const Concept& other);
    Concept(Concept&& other);
    Concept& operator=(Concept&& other);
    ~Concept() override;

    ConceptDenotation evaluate(const State& state) const;
    ConceptDenotation* evaluate(const State& state, DenotationsCaches& caches) const;
    ConceptDenotations* evaluate(const States& states, DenotationsCaches& caches) const;

    int compute_complexity() const override;

    std::string compute_repr() const override;

    const element::Concept& get_element_ref() const;
    std::shared_ptr<const element::Concept> get_element() const;
};


/**
 * Concept evaluates to RoleDenotation.
 */
class Role : public BaseElement {
private:
    std::shared_ptr<const element::Role> m_element;

    Role(std::shared_ptr<const VocabularyInfo> vocabulary_info, std::shared_ptr<const element::Role>&& role, int index=-1);
    friend class SyntacticElementFactoryImpl;

public:
    Role(const Role& other);
    Role& operator=(const Role& other);
    Role(Role&& other);
    Role& operator=(Role&& other);
    ~Role() override;

    RoleDenotation evaluate(const State& state) const;
    RoleDenotation* evaluate(const State& state, DenotationsCaches& caches) const;
    RoleDenotations* evaluate(const States& states, DenotationsCaches& caches) const;

    int compute_complexity() const override;

    std::string compute_repr() const override;

    const element::Role& get_element_ref() const;
    std::shared_ptr<const element::Role> get_element() const;
};


/**
 * Numerical evaluates to int.
 */
class Numerical : public BaseElement {
private:
    std::shared_ptr<const element::Numerical> m_element;

    Numerical(std::shared_ptr<const VocabularyInfo> vocabulary_info, std::shared_ptr<const element::Numerical>&& numerical, int index=-1);
    friend class SyntacticElementFactoryImpl;

public:
    Numerical(const Numerical& other);
    Numerical& operator=(const Numerical& other);
    Numerical(Numerical&& other);
    Numerical& operator=(Numerical&& other);
    ~Numerical() override;

    int evaluate(const State& state) const;
    int evaluate(const State& state, DenotationsCaches& caches) const;
    NumericalDenotations* evaluate(const States& states, DenotationsCaches& caches) const;

    int compute_complexity() const override;

    std::string compute_repr() const override;

    const element::Numerical& get_element_ref() const;
    std::shared_ptr<const element::Numerical> get_element() const;
};


/**
 * Boolean evaluates to bool.
 */
class Boolean : public BaseElement {
private:
    std::shared_ptr<const element::Boolean> m_element;

    Boolean(std::shared_ptr<const VocabularyInfo> vocabulary_info, std::shared_ptr<const element::Boolean>&& boolean, int index=-1);
    friend class SyntacticElementFactoryImpl;

public:
    Boolean(const Boolean& other);
    Boolean& operator=(const Boolean& other);
    Boolean(Boolean&& other);
    Boolean& operator=(Boolean&& other);
    ~Boolean() override;

    bool evaluate(const State& state) const;
    bool evaluate(const State& state, DenotationsCaches& caches) const;
    BooleanDenotations* evaluate(const States& states, DenotationsCaches& caches) const;

    int compute_complexity() const override;

    std::string compute_repr() const override;

    const element::Boolean& get_element_ref() const;
    std::shared_ptr<const element::Boolean> get_element() const;
};


/**
 * The SyntacticElementFactory for creation of syntactically unique elements.
 */
class SyntacticElementFactory {
private:
    utils::pimpl<SyntacticElementFactoryImpl> m_pImpl;

public:
    SyntacticElementFactory(std::shared_ptr<const VocabularyInfo> vocabulary_info);
    SyntacticElementFactory(const SyntacticElementFactory& other);
    SyntacticElementFactory& operator=(const SyntacticElementFactory& other);
    SyntacticElementFactory(SyntacticElementFactory&& other);
    SyntacticElementFactory& operator=(SyntacticElementFactory&& other);
    ~SyntacticElementFactory();

    const VocabularyInfo& get_vocabulary_info_ref() const;
    std::shared_ptr<const VocabularyInfo> get_vocabulary_info() const;

    /**
     * Returns a Concept if the description is correct.
     * If description is incorrect, throw an error with human readable information.
     */
    Concept parse_concept(const std::string &description, int index=-1);

    /**
     * Returns a Role if the description is correct.
     * If description is incorrect, throw an error with human readable information.
     */
    Role parse_role(const std::string &description, int index=-1);

    /**
     * Returns a Numerical if the description is correct.
     * If description is incorrect, throw an error with human readable information.
     */
    Numerical parse_numerical(const std::string &description, int index=-1);

    /**
     * Returns a Boolean if the description is correct.
     * If description is incorrect, throw an error with human readable information.
     */
    Boolean parse_boolean(const std::string &description, int index=-1);


    Boolean make_empty_boolean(const Concept& concept, int index=-1);
    Boolean make_empty_boolean(const Role& role, int index=-1);
    Boolean make_inclusion_boolean(const Concept& concept_left, const Concept& concept_right, int index=-1);
    Boolean make_inclusion_boolean(const Role& role_left, const Role& role_right, int index=-1);
    Boolean make_nullary_boolean(const Predicate& predicate, int index=-1);

    Concept make_all_concept(const Role& role, const Concept& concept, int index=-1);
    Concept make_and_concept(const Concept& concept_left, const Concept& concept_right, int index=-1);
    Concept make_bot_concept(int index=-1);
    Concept make_diff_concept(const Concept& concept_left, const Concept& concept_right, int index=-1);
    Concept make_equal_concept(const Role& role_left, const Role& role_right, int index=-1);
    Concept make_not_concept(const Concept& concept, int index=-1);
    Concept make_one_of_concept(const Constant& constant, int index=-1);
    Concept make_or_concept(const Concept& concept_left, const Concept& concept_right, int index=-1);
    Concept make_projection_concept(const Role& role, int pos, int index=-1);
    Concept make_primitive_concept(const Predicate& predicate, int pos, int index=-1);
    Concept make_some_concept(const Role& role, const Concept& concept, int index=-1);
    Concept make_subset_concept(const Role& role_left, const Role& role_right, int index=-1);
    Concept make_top_concept(int index=-1);

    Numerical make_concept_distance_numerical(const Concept& concept_from, const Role& role, const Concept& concept_to, int index=-1);
    Numerical make_count_numerical(const Concept& concept, int index=-1);
    Numerical make_count_numerical(const Role& role, int index=-1);
    Numerical make_role_distance_numerical(const Role& role_from, const Role& role, const Role& role_to, int index=-1);
    Numerical make_sum_concept_distance_numerical(const Concept& concept_from, const Role& role, const Concept& concept_to, int index=-1);
    Numerical make_sum_role_distance_numerical(const Role& role_from, const Role& role, const Role& role_to, int index=-1);

    Role make_and_role(const Role& role_left, const Role& role_right, int index=-1);
    Role make_compose_role(const Role& role_left, const Role& role_right, int index=-1);
    Role make_diff_role(const Role& role_left, const Role& role_right, int index=-1);
    Role make_identity_role(const Concept& concept, int index=-1);
    Role make_inverse_role(const Role& role, int index=-1);
    Role make_not_role(const Role& role, int index=-1);
    Role make_or_role(const Role& role_left, const Role& role_right, int index=-1);
    Role make_primitive_role(const Predicate& predicate, int pos_1, int pos_2, int index=-1);
    Role make_restrict_role(const Role& role, const Concept& concept, int index=-1);
    Role make_top_role(int index=-1);
    Role make_transitive_closure(const Role& role, int index=-1);
    Role make_transitive_reflexive_closure(const Role& role, int index=-1);
};

}

#endif
