#pragma once

#include <string>

namespace OP
{
	class ShaderIncludeUtils
	{
	public:
		// Resolves all includes recursively and pastes it to the original
		// source code, just like with c++ includes
		static std::string ResolveShaderIncludes(std::string source);
	};
}