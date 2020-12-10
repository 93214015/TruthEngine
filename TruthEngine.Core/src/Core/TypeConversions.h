#pragma once
#include "pch.h"

namespace TruthEngine
{
	inline std::wstring to_wstring(std::string_view str)
	{
		return std::wstring(str.begin(), str.end());
	}

	inline std::wstring to_wstring(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

}
