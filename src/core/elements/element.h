#ifndef DLPLAN_SRC_CORE_ELEMENTS_ELEMENT_H_
#define DLPLAN_SRC_CORE_ELEMENTS_ELEMENT_H_

#include "../../../include/dlplan/types.h"
#include "../instance_info.h"
#include "../vocabulary_info.h"
#include "../cache.h"

#include <iostream>
#include <sstream>


namespace dlplan::core::element {


template<typename T>
class Element {
protected:
    const std::string m_name;
    // index = -1 if unregistered in cache
    int m_index;

public:
    // Elements are not copieable because they must live in the cache.
    Element(const VocabularyInfo&, const std::string& name) : m_name(name), m_index(-1) { }
    Element(const Element& other) = delete;
    Element& operator=(const Element& other) = delete;
    Element(Element&& other) = delete;
    Element& operator=(Element&& other) = delete;
    virtual ~Element() = default;

    virtual T evaluate(const State& state) const = 0;

    virtual T evaluate(EvaluationContext& context) const = 0;

    virtual int compute_complexity() const = 0;

    virtual void compute_repr(std::stringstream& out) const = 0;

    virtual std::string compute_repr() const {
        std::stringstream ss;
        compute_repr(ss);
        return ss.str();
    }

    void set_index(int index) {
        assert(index >= 0);
        m_index = index;
    }

    int get_index() const {
        return m_index;
    }
};

}

#endif
