#include "core.h"

namespace dlplan::core {

template<typename T>
Element<T>::Element(
    std::shared_ptr<const VocabularyInfo> vocabulary_info)
    : m_vocabulary_info(vocabulary_info) { }

template<typename T>
Element<T>::~Element() { }

template<typename T>
std::shared_ptr<const VocabularyInfo> Element<T>::get_vocabulary_info() const {
    return m_vocabulary_info;
}

template class Element<dynamic_bitset::DynamicBitset<unsigned int>>;
template class Element<int>;
template class Element<bool>;


template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>::PerElementEvaluationCache() = default;


template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>::PerElementEvaluationCache(const PerElementEvaluationCache<Element, Denotation>& other) = default;

template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>& PerElementEvaluationCache<Element, Denotation>::operator=(const PerElementEvaluationCache<Element, Denotation>& other) = default;

template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>::PerElementEvaluationCache(PerElementEvaluationCache<Element, Denotation>&& other) = default;

template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>& PerElementEvaluationCache<Element, Denotation>::operator=(PerElementEvaluationCache<Element, Denotation>&& other) = default;

template<typename Element, typename Denotation>
PerElementEvaluationCache<Element, Denotation>::~PerElementEvaluationCache() = default;

template<typename Element, typename Denotation>
typename phmap::flat_hash_map<int, Denotation>::const_iterator
PerElementEvaluationCache<Element, Denotation>::find(int index, std::shared_ptr<const State> state) const {
    if (state != m_cached_state) {
        m_cached_state = state;
        m_element_idx_to_denotation.clear();
        return m_element_idx_to_denotation.end();
    }
    return m_element_idx_to_denotation.find(index);
}

template<typename Element, typename Denotation>
typename phmap::flat_hash_map<int, Denotation>::const_iterator
PerElementEvaluationCache<Element, Denotation>::end() const {
    return m_element_idx_to_denotation.end();
}

template<typename Element, typename Denotation>
std::pair<typename phmap::flat_hash_map<int, Denotation>::iterator, bool>
PerElementEvaluationCache<Element, Denotation>::insert(std::pair<int, Denotation>&& val) {
    return m_element_idx_to_denotation.insert(std::move(val));
}

}
