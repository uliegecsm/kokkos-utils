# Check that VARIABLE is a list of LENGTH elements.
function(check_list_length VARIABLE LENGTH)
    list(LENGTH ${VARIABLE} ${VARIABLE}_LENGTH)
    if(NOT ${VARIABLE}_LENGTH EQUAL ${LENGTH})
        message(FATAL_ERROR "List ${VARIABLE} is of length ${${VARIABLE}_LENGTH}, expecting ${LENGTH}.")
    endif()
endfunction()
