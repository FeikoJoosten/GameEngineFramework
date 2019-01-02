#pragma once

#include <new>

// We need to define these because of EASTL
// so we don't crash in operator delete
// see https://rawgit.com/electronicarts/EASTL/master/doc/EASTL%20FAQ.html#Info.15
void *operator new(std::size_t count);
void *operator new[](std::size_t count);
void *operator new(std::size_t count, std::nothrow_t const &tag) noexcept;
void *operator new[](std::size_t count, std::nothrow_t const &tag) noexcept;

void *operator new[](
	std::size_t size,
	std::size_t alignment,
	std::size_t /*alignmentOffset*/,
	char const* /*name*/,
	int /*flags*/,
	unsigned /*debugFlags*/,
	char const* /*file*/,
	int /*line*/);

void *operator new[](
	std::size_t size,
	char const* /*name*/,
	int /*flags*/,
	unsigned /*debugFlags*/,
	char const* /*file*/,
	int /*line*/);

void operator delete(void *ptr) noexcept;
void operator delete[](void *ptr) noexcept;
void operator delete(void *ptr, std::size_t count) noexcept;
void operator delete[](void *ptr, std::size_t count) noexcept;

// Seems redundant, since delete is already non-throwing, but it's in the spec.
void operator delete(void *ptr, std::nothrow_t const &tag) noexcept;
void operator delete[](void *ptr, std::nothrow_t const &tag) noexcept;
// --