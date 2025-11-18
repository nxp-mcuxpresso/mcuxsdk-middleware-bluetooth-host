/***********************************************************************************/
/*!
 *  @brief
 *  @file       VectorTypes.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#ifndef VECTOR_TYPES_H_
#define VECTOR_TYPES_H_

#include <common/include/memory/heap_allocator.h>
#include <vector>

namespace ranging {

// Using-alias to simplify code
// template <typename T>
// using vector_type = std::vector<T, VectorAllocator<T> >;

// Using-alias to simplify code
// using float_vector = vector_type<float>;
// using complex_vector = vector_type<ComplexF>;


template<typename T>
struct VectorAllocatorType {
	typedef std::vector<T, VectorAllocator<T> > type;
};

using float_vector = VectorAllocatorType<float>::type;
using complex_vector = VectorAllocatorType<ComplexF>::type;

/* bool_vector uses default allocator otherwise older gcc complains */
//using bool_vector = std::vector<bool>;
using bool_vector = VectorAllocatorType<bool>::type;
}


#endif /* VECTOR_TYPES_H_ */
