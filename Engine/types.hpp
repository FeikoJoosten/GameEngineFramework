#ifndef RTS_COMMON_TYPES_HPP
#define RTS_COMMON_TYPES_HPP

#include <cstdint>

namespace Engine
{
namespace Types
{
typedef std::int8_t			s8;
typedef std::int_fast8_t	sf8;
typedef std::int_least8_t	sl8;

typedef std::uint8_t		u8;
typedef std::uint_fast8_t	uf8;
typedef std::uint_least8_t	ul8;

typedef std::int16_t		s16;
typedef std::int_fast16_t	sf16;
typedef std::int_least16_t	sl16;

typedef std::uint16_t		u16;
typedef std::uint_fast16_t	uf16;
typedef std::uint_least16_t	ul16;

typedef std::int32_t		s32;
typedef std::int_fast32_t	sf32;
typedef std::int_least32_t	sl32;

typedef std::uint32_t		u32;
typedef std::uint_fast32_t	uf32;
typedef std::uint_least32_t	ul32;

typedef std::int64_t		s64;
typedef std::int_fast64_t	sf64;
typedef std::int_least64_t	sl64;

typedef std::uint64_t		u64;
typedef std::uint_fast64_t	uf64;
typedef std::uint_least64_t	ul64;

typedef std::uintptr_t		uintptr;
}
}

#endif // RTS_COMMON_TYPES_HPP
