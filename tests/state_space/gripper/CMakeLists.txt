add_executable(
    state_space_gripper_tests
)
target_sources(
    state_space_gripper_tests
    PRIVATE
        gripper.cpp
)

add_custom_target(state_space_delivery_domain ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/domain.pddl" "${CMAKE_BINARY_DIR}/tests/state_space/gripper/domain.pddl")
add_custom_target(state_space_delivery_instance ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/p-1-0.pddl" "${CMAKE_BINARY_DIR}/tests/state_space/gripper/p-1-0.pddl")
add_custom_target(state_space_delivery_fd ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/libs/scorpion/fast-downward.py" "${CMAKE_BINARY_DIR}/tests/state_space/gripper/fast-downward.py")

target_link_libraries(state_space_gripper_tests dlplanstatespace dlplancore gtest_main)
gtest_discover_tests(state_space_gripper_tests)