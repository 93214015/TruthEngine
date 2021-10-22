#pragma once

#define TE TruthEngine

#define TE_FLAG_TYPE uint32_t

#define TE_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) throw() { return (ENUMTYPE)( (TE_FLAG_TYPE)(a) | (TE_FLAG_TYPE)(b) ); } \
inline ENUMTYPE& operator |= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE&)( (TE_FLAG_TYPE&)(a) |= (TE_FLAG_TYPE)(b) ); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) throw() { return (ENUMTYPE)( (TE_FLAG_TYPE)(a) & (TE_FLAG_TYPE)(b) );; } \
inline ENUMTYPE& operator &= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE&)( (TE_FLAG_TYPE&)(a) &= (TE_FLAG_TYPE)(b) ); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) throw() { return (ENUMTYPE)(~((TE_FLAG_TYPE)(a))); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) throw() { return (ENUMTYPE)( (TE_FLAG_TYPE)(a) ^ (TE_FLAG_TYPE)(b) ); } \
inline ENUMTYPE& operator ^= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE&)( (TE_FLAG_TYPE&)(a) ^= (TE_FLAG_TYPE)(b) ); } \
