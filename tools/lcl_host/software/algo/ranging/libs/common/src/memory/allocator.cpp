/***********************************************************************************/
/*!
 *  @brief
 *  @file       allocator.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#include <common/include/memory/allocator.hpp>

namespace memory {
	HeapAllocator HeapAllocator::Instance;
}