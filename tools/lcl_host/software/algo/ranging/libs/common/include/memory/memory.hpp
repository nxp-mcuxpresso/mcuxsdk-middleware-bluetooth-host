// Copyright (c) 2019 imec. All rights reserved.
// Redistribution and use in source or binary form,
// with or without modification is prohibited.

#pragma once

#include <common/include/noncopyable.hpp>
#include <cstdint>
#include <cstdlib>

namespace memory {
	class Allocator;

	// Heap memory allocation, when running the algorithm on a PC for testing purposes
	void* Malloc(int size) ;
	void Free(void *ptr);

}
