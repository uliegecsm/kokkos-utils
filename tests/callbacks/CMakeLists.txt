set_property(GLOBAL APPEND PROPERTY KokkosUtils_FILES_FOR_DOC ${CMAKE_CURRENT_SOURCE_DIR}/TestWorkload.hpp)

add_one_test(NAME Events)
add_one_test(NAME Helpers)
add_one_test(NAME Manager)
add_one_test(NAME Matchers)
add_one_test(NAME RecorderListener)
