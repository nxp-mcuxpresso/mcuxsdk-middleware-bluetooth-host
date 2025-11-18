/***********************************************************************************/
/*!
 *  @brief
 *  @file       allocator.hpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#pragma once

#include <common/include/memory/memory.hpp>


namespace memory {

// Abstract base class for allocating dynamic memory
class Allocator {
public:
	// Allocate/Deallocate raw memory
	virtual void* Allocate(int size) = 0;
	virtual void DeAllocate(void *ptr, int size) = 0;

protected:

	virtual ~Allocator() {
	}
};

// Allocates memory from the system heap
class HeapAllocator: public Allocator {
public:
	virtual void* Allocate(int size) {
          return memory::Malloc(size);
	}

	virtual void DeAllocate(void *ptr, int size) {
          memory::Free(ptr);
	}

	static HeapAllocator Instance;
};

}

