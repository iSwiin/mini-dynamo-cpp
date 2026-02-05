function(enable_sanitizers target_name)
  if (MSVC)
    # Sanitizers via MSVC/clang-cl are different; keep simple for now.
    return()
  endif()

  if (ENABLE_ASAN)
    target_compile_options(${target_name} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${target_name} PRIVATE -fsanitize=address)
  endif()

  if (ENABLE_UBSAN)
    target_compile_options(${target_name} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
    target_link_options(${target_name} PRIVATE -fsanitize=undefined)
  endif()
endfunction()
