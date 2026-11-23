#include "./memory.hh"

namespace arcane {

	void* HeapAllocator::allocate(u64 size, u64 alignment) {
		return std::aligned_alloc(alignment, size);
	}

	void HeapAllocator::deallocate(void* pointer) {
		return std::free(pointer);
	}

}
