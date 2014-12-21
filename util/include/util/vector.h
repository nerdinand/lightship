#ifndef LIGHTSHIP_UTIL_VECTOR_HPP
#define LIGHTSHIP_UTIL_VECTOR_HPP

#define DATA_POINTER_TYPE unsigned char*
struct vector_t
{
    int element_size;       /* how large one element is in bytes */
    int capacity;           /* how many elements actually fit into the allocated space */
    int count;              /* number of elements inserted */
    DATA_POINTER_TYPE data; /* pointer to the contiguous section of memory */
};

/*!
 * @brief Creates a new vector object.
 * @param [in] element_size Specifies the size in bytes of the type of data you want
 * the vector to store. Typically one would pass sizeof(my_data_type).
 * @return Returns the newly created vector object.
 */
struct vector_t* vector_create(const int element_size);

/*!
 * @brief Initialises an existing vector object.
 * @note This does **not** FREE existing memory. If you've pushed elements
 * into your vector and call this, you will have created a memory leak.
 * @param [in] vector The vector to initialise.
 * @param [in] element_size Specifies the size in bytes of the type of data you
 * want the vector to store. Typically one would pass sizeof(my_data_type).
 */
void vector_init_vector(struct vector_t* vector, const int element_size);

/*!
 * @brief Destroys an existing vector object and FREEs all memory allocated by
 * inserted elements.
 * @param [in] vector The vector to destroy.
 */
void vector_destroy(struct vector_t* vector);

/*!
 * @brief Erases all elements in a vector.
 * @note This does not actually erase the underlying memory, it simply resets
 * the element counter. If you wish to FREE the underlying memory, see
 * vector_clear_FREE().
 * @param [in] vector The vector to clear.
 */
void vector_clear(struct vector_t* vector);

/*!
 * @brief Erases all elements in a vector and FREEs their memory.
 * @param [in] vector The vector to clear.
 */
void vector_clear_FREE(struct vector_t* vector);

/*!
 * @brief Gets the number of elements that have been inserted into the vector.
 */
#define vector_count(x) ((x)->count)

/*!
 * @brief Inserts (copies) a new element at the head of the vector.
 * @note This can cause a re-allocation of the underlying memory. This
 * implementation expands the allocated memory by a factor of 2 every time a
 * re-allocation occurs to cut down on the frequency of re-allocations.
 * @note If you do not wish to copy data into the vector, but merely make
 * space, see vector_push_emplace().
 * @param [in] vector The vector to push into.
 * @param [in] data The data to copy into the vector. It is assumed that
 * sizeof(data) is equal to what was specified when the vector was first
 * created. If this is not the case then it could cause undefined behaviour.
 */
void vector_push(struct vector_t* vector, void* data);

/*!
 * @brief Allocates space for a new element at the head of the vector, but does
 * not initialise it.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param [in] vector The vector to emplace an element into.
 * @return A pointer to the allocated memory for the requested element. See
 * warning and use with caution.
 */
void* vector_push_emplace(struct vector_t* vector);

/*!
 * @brief Removes an element from the head of the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param [in] vector The vector to pop an element from.
 * @return A pointer to the popped element. See warning and use with caution.
 * If there are no elements to pop, NULL is returned.
 */
void* vector_pop(struct vector_t* vector);

/*!
 * @brief Inserts (copies) a new element into the middle of the vector.
 * @note This can cause a re-allocation of the underlying memory. This
 * implementation expands the allocated memory by a factor of 2 every time a
 * re-allocation occurs to cut down on the frequency of re-allocations.
 * @param [in] vector The vector to insert into.
 * @param [in] index The position in the vector to insert into. The index
 * ranges from **0** to **vector_count()-1**.
 * @param [in] data The data to copy into the vector. It is assumed that
 * sizeof(data) is equal to what was specified when the vector was first
 * created. If this is not the case then it could cause undefined behaviour.
 */
void vector_insert(struct vector_t* vector, int index, void* data);

/*!
 * @brief Erases the specified element from the vector.
 * @note This causes all elements with indices greater than **index** to be
 * re-allocated (shifted 1 element down) so the vector remains contiguous.
 * @param [in] index The position of the element in the vector to erase. The index
 * ranges from **0** to **vector_count()-1**.
 */
void vector_erase(struct vector_t* vector, int index);

/*!
 * @brief Gets a pointer to the specified element in the vector.
 * @note **WARNING** The returned pointer could be invalidated if any other
 * vector related function is called, as the underlying memory of the vector
 * could be re-allocated. Use the pointer immediately after calling this
 * function.
 * @param [in] vector The vector to get the element from.
 * @param [in] index The index of the element to get. The index ranges from
 * **0** to **vector_count()-1**.
 * @return [in] A pointer to the element. See warning and use with caution.
 * If the specified element doesn't exist (index out of bounds), NULL is
 * returned.
 */
void* vector_get_element(struct vector_t*, int index);

/*!
 * @brief Convenient macro for iterating a vector's elements.
 * 
 * Example:
 * @code
 * vector_t* someVector = (a vector containing elements of type "struct bar")
 * LIST_FOR_EACH(someList, struct bar, element)
 * {
 *     do_something_with(element);  ("element" is now of type "struct bar*")
 * }
 * @endcode
 * @param [in] vector The vector to iterate.
 * @param [in] var_type Should be the type of data stored in the vector.
 * @param [in] var The name of a temporary variable you'd like to use within the
 * for-loop to reference the current element.
 */
#define VECTOR_FOR_EACH(vector, var_type, var) \
    var_type* var; \
    void* end_of_vector = vector->data + vector->count * vector->element_size; \
    for(var = vector->data; var != end_of_vector; var += vector->element_size)

#endif /* LIGHTSHIP_UTIL_VECTOR_HPP */
