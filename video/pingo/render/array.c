#include "array.h"
#include "state.h"

int array_init(Array *array, size_t size, void *data)
{
    // Check for null pointer to avoid segmentation fault
    IF_NULL_RETURN(array, INIT_ERROR);

    array->size = size;

    if (size == 0) {
        data = NULL; // Set data to NULL if size is 0
        array->data = data;
        return OK;
    }

    // Check for null data pointer when size is not 0
    IF_NULL_RETURN(data, INIT_ERROR);
    array->data = data;

    return OK;
}
