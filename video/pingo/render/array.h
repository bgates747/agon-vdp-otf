#pragma once

#include <stddef.h>

/// @brief Array structure representing a generic array with a size and data pointer.
typedef struct {
    size_t size; ///< Size of the array
    void *data;  ///< Pointer to the array data
} Array;

/// @brief Initializes an array with the given size and data.
/// @param array Pointer to the array to initialize.
/// @param size Size of the array.
/// @param data Pointer to the array data. Can be NULL if size is 0.
/// @return Status code indicating success or error.
int array_init(Array *array, size_t size, void *data);
