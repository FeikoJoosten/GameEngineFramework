#include "Engine/Utility/NewOverrides.hpp"

#include <malloc.h>

namespace
{
	void *allocate(std::size_t size, std::size_t alignment = sizeof(void*))
	{
		void *allocatedMemory = nullptr;

		do
		{
			allocatedMemory = _aligned_malloc(size, sizeof(void*));

			if (!allocatedMemory)
			{
				std::new_handler handler = std::get_new_handler();

				if (handler)
					handler();
				else
					throw std::bad_alloc();
			}
		} while (!allocatedMemory);

		return allocatedMemory;
	}
}

void *operator new(std::size_t count)
{
	return allocate(count);
}

void *operator new[](std::size_t count)
{
	return ::operator new(count);
}

void *operator new(std::size_t count, std::nothrow_t const &tag) noexcept
{
	(void)tag;

	return _aligned_malloc(count, sizeof(void*));
}

void *operator new[](std::size_t count, std::nothrow_t const &tag) noexcept
{
	return operator new(count, tag);
}

void *operator new[](
	std::size_t size,
	std::size_t alignment,
	std::size_t /*alignmentOffset*/,
	char const* /*name*/,
	int /*flags*/,
	unsigned /*debugFlags*/,
	char const* /*file*/,
	int /*line*/)
{
	return allocate(size, alignment);
}

void *operator new[](
	std::size_t size,
	char const* name,
	int flags,
	unsigned debugFlags,
	char const* file,
	int line)
{
	return operator new[](
		size,
		sizeof(void*),
		0,
		name,
		flags,
		debugFlags,
		file,
		line);
}

void operator delete(void *ptr) noexcept
{
	_aligned_free(ptr);
}

void operator delete[](void *ptr) noexcept
{
	operator delete(ptr);
}

void operator delete(void *ptr, std::size_t count) noexcept
{
	(void)count;
	_aligned_free(ptr);
}

void operator delete[](void *ptr, std::size_t count) noexcept
{
	operator delete(ptr, count);
}

void operator delete(void *ptr, std::nothrow_t const &tag) noexcept
{
	(void)tag;
	_aligned_free(ptr);
}

void operator delete[](void *ptr, std::nothrow_t const &tag) noexcept
{
	::operator delete(ptr, tag);
}