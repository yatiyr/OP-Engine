#pragma once

#include <filesystem>

#include <Renderer/Texture.h>

namespace Opium
{
	class ContentBrowserComponent
	{
	public:
		ContentBrowserComponent();
		void OnImGuiRender();

	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
	};
}