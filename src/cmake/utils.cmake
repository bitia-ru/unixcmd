function(check_starts_with str_var prefix result)
    string(LENGTH "${prefix}" prefix_length)
    string(SUBSTRING "${${str_var}}" 0 "${prefix_length}" str_prefix)

    set(${result} FALSE PARENT_SCOPE)
    if(${str_prefix} STREQUAL ${prefix})
        set(${result} TRUE PARENT_SCOPE)
    endif()
endfunction()
