#ifndef DLPLAN_SRC_CORE_CACHE_H_
#define DLPLAN_SRC_CORE_CACHE_H_

#include <unordered_map>
#include <memory>
#include <iostream>
#include <cassert>
#include <mutex>

#include "../utils/cache.h"

#include "../../include/dlplan/core.h"


namespace dlplan::core {

/**
 * A thread-safe reference-counted object cache.
 * Idea taken from Herb Sutter: https://channel9.msdn.com/Events/GoingNative/2013/My-Favorite-Cpp-10-Liner
 * Other sources: (1) https://stackoverflow.com/questions/49782011/herb-sutters-10-liner-with-cleanup
 */
template<typename KEY, typename VALUE>
class ReferenceCountedObjectCache : public std::enable_shared_from_this<ReferenceCountedObjectCache<KEY, VALUE>> {
private:
    std::unordered_map<KEY, std::weak_ptr<VALUE>> m_cache;
    mutable std::mutex m_mutex;

    int m_count;

public:
    ReferenceCountedObjectCache() : m_count(0) { }

    /**
     * Retrieves a certain element.
     */
    std::shared_ptr<VALUE> at(const KEY& key) {
        std::lock_guard<std::mutex> hold(m_mutex);
        return m_cache.at(key).lock();
    }

    /**
     * Inserts a new value and derives the key from it.
     */
    std::pair<std::shared_ptr<VALUE>, bool> insert(std::unique_ptr<VALUE>&& element) {
        KEY key = element->compute_repr();
        /* we must declare sp before locking the mutex
           s.t. the deleter is called after the mutex was released in case of stack unwinding. */
        std::shared_ptr<VALUE> sp;
        std::lock_guard<std::mutex> hold(m_mutex);
        auto& cached = m_cache[key];
        sp = cached.lock();
        bool new_insertion = false;
        if (!sp) {
            new_insertion = true;
            element->set_index(m_count++);
            cached = sp = std::shared_ptr<VALUE>(
                element.get(),
                [parent=this->shared_from_this(), original_deleter=element.get_deleter()](VALUE* x)
                {
                    {
                        std::lock_guard<std::mutex> hold(parent->m_mutex);
                        parent->m_cache.erase(x->compute_repr());
                    }
                    /* After cache removal, we can call the objects destructor
                       and recursively call the deleter of children if their ref count goes to 0 */
                    original_deleter(x);
                }
            );
            element.release();
        }
        return std::make_pair(sp, new_insertion);
    }

    size_t size() const {
        std::lock_guard<std::mutex> hold(m_mutex);
        return m_cache.size();
    }
};

/**
 * One cache for each template instantiated element.
 */
struct Caches {
    std::shared_ptr<ReferenceCountedObjectCache<std::string, element::Concept>> m_concept_cache;
    std::shared_ptr<ReferenceCountedObjectCache<std::string, element::Role>> m_role_cache;
    std::shared_ptr<ReferenceCountedObjectCache<std::string, element::Numerical>> m_numerical_cache;
    std::shared_ptr<ReferenceCountedObjectCache<std::string, element::Boolean>> m_boolean_cache;

    Caches()
        : m_concept_cache(std::make_shared<ReferenceCountedObjectCache<std::string, element::Concept>>()),
          m_role_cache(std::make_shared<ReferenceCountedObjectCache<std::string, element::Role>>()),
          m_numerical_cache(std::make_shared<ReferenceCountedObjectCache<std::string, element::Numerical>>()),
          m_boolean_cache(std::make_shared<ReferenceCountedObjectCache<std::string, element::Boolean>>()) { }
};

}

#endif