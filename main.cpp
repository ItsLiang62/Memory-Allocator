#include <cstddef>
#include <memory>
#include <new>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <vector>

class Arena {
	std::byte* buffer; // pointer to heap allocated bytes (arena)
	std::size_t size; // total capacity of arena
	std::size_t offset = 0; // added to arena pointer to get pointer to remaining arena

	public:		
		// prevent implicit constructor calls such as Arena arena = 1024
		// can cause hidden bugs or performance hits
		// good practice
		explicit Arena(std::size_t size_) : size(size_), buffer(new std::byte[size_]) {}

		~Arena() {
			delete[] buffer;
		}

		// compilers auto-generate copy constructors
		// avoid them to prevent double-free of the same arena pointer when destructors run
		Arena(const Arena&) = delete; // construct brand new copy
		Arena& operator=(const Arena&) = delete; // assign copy to an alias
		// prevent creating copies of Arena object as a result

		void* allocate(std::size_t bytes, std::size_t alignment) {
			void* buffer_rem = buffer + offset;
			std::size_t size_rem = size - offset; // to undergo alignment

			// adjust pointer to remaining arena, and remaining capacity
			// require bytes to allocate
			// to judge whether enough remaining capacity for it after alignment
			if (std::align(alignment, bytes, buffer_rem, size_rem)) {
				// new offset calculated as total capacity -
				// remaining capacity after alignment before allocation +
				// bytes to allocate
				// std::align to calculate 
				offset = size - size_rem + bytes;
				return buffer_rem;
			}

			throw std::bad_alloc();
		}

		void reset() noexcept {
			offset = 0; // allow allocation from beginning of arena
		}
};

template <typename T>
class ArenaAllocator {
	private:
		Arena* arena;

	public:
		using value_type = T;

		template <typename U>
		// conditional compile time constructor
		// allow construct using arena of existing allocator of any type
		// mandatory for node-based standard containers' allocator
		// since they convert element and allocator from type to Node<type>
		ArenaAllocator(const ArenaAllocator<U>& other) noexcept 
		: arena(other.arena) {}

		explicit ArenaAllocator(Arena& arena_) noexcept : arena(&arena_) {}

		// issue warning if return value ignored
		[[nodiscard]] T* allocate(std::size_t n) {
			if (n <= 0) return nullptr;
			// main purpose, to allocate based on allocator type
			return static_cast<T*>(arena->allocate(n * sizeof(T), alignof(T)));
		}

		void deallocate(T* p, std::size_t n) noexcept {};

		template <typename V>
		// allow other allocators, regardless their class is template or regular
		// to access private members of this class
		friend class ArenaAllocator;

		// allow check equality based on arena pointer
		bool operator==(const ArenaAllocator& other) const noexcept {
			return arena == other.arena;
		}
};

int main() {
	Arena memory_pool(1024);
	ArenaAllocator<int> allocator_int(memory_pool);

	std::vector<int, ArenaAllocator<int>> vec(allocator_int);

	for (int i=0; i<10; ++i) {
		vec.push_back(i * 10);
	}

	for (int val : vec) {
		std::cout << val << " ";
	}
	std::cout << "\n";

	memory_pool.reset();
	return 0;
}