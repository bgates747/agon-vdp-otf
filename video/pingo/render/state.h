#pragma once

/// @brief Enum representing various states for error handling.
enum State {
    OK,           ///< Ok state
    INIT_ERROR,   ///< Error during an init call
    RENDER_ERROR, ///< Error during a render call
    SET_ERROR     ///< Error during a set call
};

/// @brief Macro to check for null pointers and return a specified value if null.
/// @param ptr Pointer to check.
/// @param retval Value to return if the pointer is null.
#define IF_NULL_RETURN(ptr, retval)                                            \
  if ((ptr) == 0)                                                              \
    return retval;
