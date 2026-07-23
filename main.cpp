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
};

int main() {

}