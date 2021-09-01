#pragma once

namespace TruthEngine
{
	class EditorEntityIconComponent
	{
	public:
		uint32_t GetTextureIndex() { return m_TextureIndex; }
	private:
		uint32_t m_TextureIndex;
	};
}