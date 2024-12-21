cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_COMPILER "gcc")

project(my_project)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB BASE64_SRC_FILES ${PROJECT_SOURCE_DIR}/lib/base64/src/base64.c)
file(GLOB DYNARR_SRC_FILES ${PROJECT_SOURCE_DIR}/lib/dynarr/src/dynarr.c)
file(GLOB STR_SRC_FILES ${PROJECT_SOURCE_DIR}/lib/str/src/str.c)
file(GLOB HFM_SRC_FILES ${PROJECT_SOURCE_DIR}/lib/hfm/src/hfm.c)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.c)

add_executable(
        test_base64
        test/test_base64/test_base64.c
        ${BASE64_SRC_FILES}
        ${DYNARR_SRC_FILES}
)
target_include_directories(test_base64 PRIVATE ${PROJECT_SOURCE_DIR}/lib/base64/include)
target_include_directories(test_base64 PRIVATE ${PROJECT_SOURCE_DIR}/lib/dynarr/include)


add_executable(
        test_dynarr
        test/test_dynarr/test_dynarr.c
        ${DYNARR_SRC_FILES}
)
target_include_directories(test_dynarr PRIVATE ${PROJECT_SOURCE_DIR}/lib/dynarr/include)

add_executable(
        test_str
        test/test_str/test_str.c
        ${STR_SRC_FILES}
)
target_include_directories(test_str PRIVATE ${PROJECT_SOURCE_DIR}/lib/str/include)

add_executable(
        test_hfm
        test/test_hfm/test_hfm.c
        ${HFM_SRC_FILES}
)
target_include_directories(test_hfm PRIVATE ${PROJECT_SOURCE_DIR}/lib/hfm/include)

add_test(
        NAME test_base64
        COMMAND $<TARGET_FILE:test_base64>
)

add_test(
        NAME test_dynarr
        COMMAND $<TARGET_FILE:test_dynarr>
)

add_test(
        NAME test_str
        COMMAND $<TARGET_FILE:test_str>
)

add_test(
        NAME test_hfm
        COMMAND $<TARGET_FILE:test_hfm>
)

enable_testing()