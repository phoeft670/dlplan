#include <gtest/gtest.h>

#include "../include/dlplan/core.h"

using namespace dlplan::core;


TEST(DLPTests, ConceptProjection) {
    // Add predicates
    std::shared_ptr<VocabularyInfo> vocabulary = std::make_shared<VocabularyInfo>();
    Predicate p0 = vocabulary->add_predicate("predicate", 2);
    std::shared_ptr<InstanceInfo> instance = std::make_shared<InstanceInfo>(vocabulary);
    // Add state atoms
    Atom a0 = instance->add_atom("predicate", {"A", "B"});
    Atom a1 = instance->add_atom("predicate", {"C", "D"});

    State state(instance, {a0, a1});

    SyntacticElementFactory factory(vocabulary);

    Concept concept1 = factory.parse_concept("c_projection(predicate(0, 1), 0)");
    EXPECT_EQ(concept1.evaluate(state), dlplan::core::ConceptDenotation({0, 2}));

    Concept concept2 = factory.parse_concept("c_projection(predicate(1, 0), 0)");
    EXPECT_EQ(concept2.evaluate(state), dlplan::core::ConceptDenotation({1, 3}));

    Concept concept3 = factory.parse_concept("c_projection(predicate(0, 1), 1)");
    EXPECT_EQ(concept3.evaluate(state), dlplan::core::ConceptDenotation({1, 3}));

    Concept concept4 = factory.parse_concept("c_projection(predicate(1, 0), 1)");
    EXPECT_EQ(concept4.evaluate(state), dlplan::core::ConceptDenotation({0, 2}));
}
