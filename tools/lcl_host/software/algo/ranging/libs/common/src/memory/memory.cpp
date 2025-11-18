// Copyright (c) 2019 imec. All rights reserved.
// Redistribution and use in source or binary form,
// with or without modification is prohibited.

#include <common/include/memory/memory.hpp>

namespace memory {

void* Malloc(int size) {
	return std::malloc(size);
}

void Free(void *ptr) {
	std::free(ptr);
}

}
