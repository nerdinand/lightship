#include <string.h>
#include <stdlib.h>
#include "util/vector.h"
#include "util/memory.h"

/*!
 * @brief Expands the underlying memory.
 * 
 * This implementation will expand the memory by a factor of 2 each time this
 * is called. All elements are copied into the new section of memory.
 * @param [in] insertion_index Set to -1 if no space should be made for element
 * insertion. Otherwise this parameter specifies the index of the element to
 * "evade" when re-allocating all other elements.
 */
static void vector_expand(struct vector_t* vector, int insertion_index);

struct vector_t* vector_create(const int element_size)
{
    struct vector_t* vector = (struct vector_t*)MALLOC(sizeof(struct vector_t));
    vector_init_vector(vector, element_size);
    return vector;
}

void vector_init_vector(struct vector_t* vector, const int element_size)
{
    memset(vector, 0, sizeof(struct vector_t));
    vector->element_size = element_size;
}

void vector_destroy(struct vector_t* vector)
{
    vector_clear(vector);
    FREE(vector);
}

void vector_clear(struct vector_t* vector)
{
    /* 
     * No need to FREE or overwrite existing memory, just reset the counter
     * and let future insertions overwrite
     */
    vector->count = 0;
}

void vector_clear_FREE(struct vector_t* vector)
{
    if(vector->data)
        FREE(vector->data);
    vector->data = NULL;
    vector->count = 0;
    vector->capacity = 0;
}

void* vector_push_emplace(struct vector_t* vector)
{
    void* data;
    if(vector->count == vector->capacity)
        vector_expand(vector, -1);
    data = vector->data + (vector->element_size * vector->count);
    ++vector->count;
    return data;
}

void vector_push(struct vector_t* vector, void* data)
{
    memcpy(vector_push_emplace(vector), data, vector->element_size);
}

void* vector_pop(struct vector_t* vector)
{
    if(!vector->count)
        return NULL;

    --vector->count;
    return vector->data + (vector->element_size * vector->count);
}

void vector_insert(struct vector_t* vector, int index, void* data)
{
    int offset;

    /* 
     * Normally the last valid index is (capacity-1), but in this case it's valid
     * because it's possible the user will want to insert at the very end of
     * the vector.
     */
    if(index > vector->capacity)
        return;

    /* re-allocate? */
    if(vector->count == vector->capacity)
        vector_expand(vector, index);
    else
    {
        /* shift all elements up by one to make space for insertion */
        int total_size = vector->count * vector->element_size;
        offset = index * vector->element_size;
        memcpy(vector->data + offset + vector->element_size,
               vector->data + offset,
               total_size - offset);
    }

    /* copy new element into the specified index */
    offset = vector->element_size * index;
    memcpy(vector->data + offset, data, vector->element_size);
    ++vector->count;
}

void vector_erase(struct vector_t* vector, int index)
{
    int offset;
    int total_size;

    if(index >= vector->capacity)
        return;
    
    /* shift memory right after the specified element down by one element */
    offset = vector->element_size * index;  /* offset to the element being erased in bytes */
    total_size = vector->element_size * vector->count; /* total current size in bytes */
    memcpy(vector->data + offset,   /* target is to overwrite the element specified by index */
           vector->data + offset + vector->element_size,    /* copy beginning from one element ahead of element to be erased */
           total_size - offset - vector->element_size);     /* copying number of elements after element to be erased */
    --vector->count;
}

void* vector_get_element(struct vector_t* vector, int index)
{
    if(index >= vector->count)
        return NULL;
    return vector->data + (vector->element_size * index);
}

static void vector_expand(struct vector_t* vector, int insertion_index)
{
    int new_size;
    DATA_POINTER_TYPE old_data;
    DATA_POINTER_TYPE new_data;

    /* expand by factor 2 */
    new_size = vector->capacity << 2;
    
    /* 
     * If vector hasn't allocated anything yet, allocate the first two elements
     * and return 
     */
    if(new_size == 0)
    {
        vector->data = MALLOC(vector->element_size << 2);
        return;
    }
    
    /* prepare for reallocating data */
    old_data = vector->data;
    new_data = (DATA_POINTER_TYPE)MALLOC(vector->element_size * new_size);
    
    /* if no insertion index is required, copy all data to new memory */
    if(insertion_index == -1 || insertion_index >= new_size)
        memcpy(new_data, old_data, vector->element_size * vector->count);
    
    /* keep space for one element at the insertion index */
    else
    {
        /* copy old data up until right before insertion offset */
        int offset = vector->element_size * insertion_index;
        int total_size = vector->element_size * vector->count;
        memcpy(new_data, old_data, offset);
        /* copy the remaining amount of old data shifted one element ahead */
        memcpy(new_data + offset + vector->element_size,
               old_data + offset,
               total_size - offset);
    }
    vector->data = new_data;
    FREE(old_data);
    
    /* update counters */
    vector->capacity = new_size;
}
