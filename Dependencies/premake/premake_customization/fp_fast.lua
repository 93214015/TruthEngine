require('vstudio')

local function premakeVersionComment(prj)
	premake.w('<FloatingPointModel>Fast</FloatingPointModel>')
end

premake.override(premake.vstudio.vc2013.elements, "clCompile", function(base, prj)
	local calls = base(prj)
	table.insertafter(calls, m.additionalIncludeDirectories, premakeVersionComment)
	return calls
end)


premake.override(premake.vstudio.vc2010.elements, "clCompile", function(base, prj)
	local calls = base(prj)
	table.insertafter(calls, m.additionalIncludeDirectories, premakeVersionComment)
	return calls
end)