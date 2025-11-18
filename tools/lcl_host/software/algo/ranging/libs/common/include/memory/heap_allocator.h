/***********************************************************************************/
/*!
 *  @brief      
 *  @file       allocator.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef MEMORY_HEAP_ALLOCATOR_H_
#define MEMORY_HEAP_ALLOCATOR_H_

#include <cstdlib>
#include <new>
#include <limits>

#include <common/include/memory/memory.hpp>
#include <common/include/memory/allocator.hpp>

//#define APRINTF(fmt, ...)  do { printf(fmt, ## __VA_ARGS__); } while (0)
#define APRINTF(fmt, ...)    do { /* empty */ } while (0)

namespace ranging {

template<class T>
struct VectorAllocator {

	typedef T value_type;
	VectorAllocator() = default;
	template<class U> constexpr VectorAllocator(const VectorAllocator<U>&) noexcept { /* empty */ }
	T* allocate(std::size_t n) {
		auto &a = memory::HeapAllocator::Instance;
		auto p = static_cast<T*>(a.Allocate(n * sizeof(T)));
		APRINTF("V heap allocated %d bytes  %p\n", (int) (n * sizeof(T)), (void*) p);
		return p;
	}
	void deallocate(T *p, std::size_t n) noexcept {
		auto &a = memory::HeapAllocator::Instance;
		a.DeAllocate(p, n * sizeof(T));
		APRINTF("V heap freed %d bytes %p\n", (int) (n * sizeof(T)), (void*) p);
	}
	template<typename U>
	using rebind = VectorAllocator<U>;
};

template<class T, class U>
bool operator==(const VectorAllocator<T>&, const VectorAllocator<U>&) {
	return true;
}

template<class T, class U>
bool operator!=(const VectorAllocator<T>&, const VectorAllocator<U>&) {
	return false;
}

/* ------------------------------------------------------------------------------------------- */

template<class T>
struct MatrixAllocator {
	typedef T value_type;
	MatrixAllocator() = default;
	template<class U> constexpr MatrixAllocator(const MatrixAllocator<U>&) noexcept { /* empty */ }
	T* allocate(std::size_t n) {
		auto &a = memory::HeapAllocator::Instance;
		auto p = static_cast<T*>(a.Allocate(n * sizeof(T)));
		APRINTF("V heap allocated %d bytes  %p\n", (int) (n * sizeof(T)), (void*) p);
		return p;
	}
	void deallocate(T *p, std::size_t n) noexcept {
		auto &a = memory::HeapAllocator::Instance;
		a.DeAllocate(p, n * sizeof(T));
		APRINTF("V heap freed %d bytes %p\n", (int) (n * sizeof(T)), (void*) p);
	}
	template<typename U>
	using rebind = MatrixAllocator<U>;
};

template<class T, class U>
bool operator==(const MatrixAllocator<T>&, const MatrixAllocator<U>&) {
	return true;
}

template<class T, class U>
bool operator!=(const MatrixAllocator<T>&, const MatrixAllocator<U>&) {
	return false;
}

}

#endif /* MEMORY_HEAP_ALLOCATOR_H_ */
