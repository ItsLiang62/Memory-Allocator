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
		explicit Arena(std::size_t size_) : size(size_) {
			buffer = new std::byte[size_]; // array of bytes on heap
		}

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

int main() {

}