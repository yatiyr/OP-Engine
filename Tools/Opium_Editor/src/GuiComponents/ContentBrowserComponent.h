#pragma once

#include <filesystem>

namespace Opium
{
	class ContentBrowserComponent
	{
	public:
		ContentBrowserComponent();
		void OnImGuiRender();

	private:
		std::filesystem::path m_CurrentDirectory;
	};
}