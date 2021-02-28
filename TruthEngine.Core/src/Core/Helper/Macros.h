#pragma once

#define ENUM_BIT_OPERATIONS_DECLARATION(T) T operator|(const T lhs, const T rhs);\
										   T operator&(const T lhs, const T rhs);


#define ENUM_BIT_OPERATIONS_DEFINITION(T) inline T operator|(const T lhs, const T rhs) { return static_cast<T>(static_cast<int>(lhs) | static_cast<int>(rhs)); };\
							              inline T operator&(const T lhs, const T rhs) { return static_cast<T>(static_cast<int>(lhs) & static_cast<int>(rhs)); };
