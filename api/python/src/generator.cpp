#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // Necessary for automatic conversion of e.g. std::vectors

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include "../../../include/dlplan/generator.h"

namespace py = pybind11;

using namespace dlplan::generator;


void init_generator(py::module_ &m) {
    py::class_<FeatureGenerator, std::shared_ptr<FeatureGenerator>>(m, "FeatureGenerator")
        .def(py::init<>())
        .def("__copy__", [](const FeatureGenerator& generator, py::object){ return FeatureGenerator(generator); })
        .def("__deepcopy__", [](const FeatureGenerator& generator, py::object){ return FeatureGenerator(generator); })
        .def("generate", &FeatureGenerator::generate)
        .def("set_generate_empty_boolean", &FeatureGenerator::set_generate_empty_boolean)
        .def("set_generate_inclusion_boolean", &FeatureGenerator::set_generate_inclusion_boolean)
        .def("set_generate_nullary_boolean", &FeatureGenerator::set_generate_nullary_boolean)
        .def("set_generate_all_concept", &FeatureGenerator::set_generate_all_concept)
        .def("set_generate_and_concept", &FeatureGenerator::set_generate_and_concept)
        .def("set_generate_bot_concept", &FeatureGenerator::set_generate_bot_concept)
        .def("set_generate_diff_concept", &FeatureGenerator::set_generate_diff_concept)
        .def("set_generate_equal_concept", &FeatureGenerator::set_generate_equal_concept)
        .def("set_generate_not_concept", &FeatureGenerator::set_generate_not_concept)
        .def("set_generate_one_of_concept", &FeatureGenerator::set_generate_one_of_concept)
        .def("set_generate_or_concept", &FeatureGenerator::set_generate_or_concept)
        .def("set_generate_primitive_concept", &FeatureGenerator::set_generate_primitive_concept)
        .def("set_generate_projection_concept", &FeatureGenerator::set_generate_projection_concept)
        .def("set_generate_some_concept", &FeatureGenerator::set_generate_some_concept)
        .def("set_generate_subset_concept", &FeatureGenerator::set_generate_subset_concept)
        .def("set_generate_top_concept", &FeatureGenerator::set_generate_top_concept)
        .def("set_generate_concept_distance_numerical", &FeatureGenerator::set_generate_concept_distance_numerical)
        .def("set_generate_count_numerical", &FeatureGenerator::set_generate_count_numerical)
        .def("set_generate_and_role", &FeatureGenerator::set_generate_and_role)
        .def("set_generate_compose_role", &FeatureGenerator::set_generate_compose_role)
        .def("set_generate_diff_role", &FeatureGenerator::set_generate_diff_role)
        .def("set_generate_identity_role", &FeatureGenerator::set_generate_identity_role)
        .def("set_generate_inverse_role", &FeatureGenerator::set_generate_inverse_role)
        .def("set_generate_not_role", &FeatureGenerator::set_generate_not_role)
        .def("set_generate_or_role", &FeatureGenerator::set_generate_or_role)
        .def("set_generate_primitive_role", &FeatureGenerator::set_generate_primitive_role)
        .def("set_generate_restrict_role", &FeatureGenerator::set_generate_restrict_role)
        .def("set_generate_top_role", &FeatureGenerator::set_generate_top_role)
        .def("set_generate_transitive_closure_role", &FeatureGenerator::set_generate_transitive_closure_role)
        .def("set_generate_transitive_reflexive_closure_role", &FeatureGenerator::set_generate_transitive_reflexive_closure_role)
    ;
}
