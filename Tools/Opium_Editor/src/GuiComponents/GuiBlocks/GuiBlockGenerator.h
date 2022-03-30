#include <imgui/imgui.h>

#include <string>
#include <vector>
#include <functional>

namespace OP
{
	extern ImFont* ImGuiIconFontBg;
	extern ImFont* ImGuiIconFontMd;
	extern ImFont* ImGuiIconFontText;

	static void Button_BackgroundCol(const std::string& name,
									const ImVec4& color,
									const std::function<void()> function = []() -> void {return; },
									const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ color.x, color.y, color.z, color.w * 0.1f });


		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	static void Button_BackgroundCol_Text(const std::string& name,
									      const ImVec4& color,
									      const std::function<void()> function = []() -> void {return; },
										  const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ color.x, color.y, color.z, color.w * 0.1f });
		ImGui::PushFont(ImGuiIconFontText);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}
		
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	static void Button_BackgroundCol_MD(const std::string& name,
										const ImVec4& color,
										const std::function<void()> function = []() -> void {return; },
										const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ color.x, color.y, color.z, color.w * 0.1f });
		ImGui::PushFont(ImGuiIconFontMd);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	static void Button_BackgroundCol_BG(const std::string& name,
										const ImVec4& color,
										const std::function<void()> function = []() -> void {return; },
										const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ color.x, color.y, color.z, color.w * 0.1f });
		ImGui::PushFont(ImGuiIconFontBg);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	static void Button_Col(const std::string& name,
						   const ImVec4& color,
						   const std::function<void()> function = []() -> void {return; },
						   const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopStyleColor();
	}

	static void Button_Col_Text(const std::string& name,
								const ImVec4& color,
								const std::function<void()> function = []() -> void {return; },
								const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGuiIconFontText);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	static void Button_Col_MD(const std::string& name,
							  const ImVec4& color,
							  const std::function<void()> function = []() -> void {return; },
							  const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGuiIconFontMd);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	static void Button_Col_BG(const std::string& name,
							  const ImVec4& color,
							  const std::function<void()> function = []() -> void {return; },
							  const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGuiIconFontBg);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	static void Button(const std::string& name,
					   const std::function<void()> function = []() -> void {return; },
					   const ImVec2& size = ImVec2(0, 0))
	{
		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}
	}

	static void Button_Text(const std::string& name,
							const std::function<void()> function = []() -> void {return; },
							const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushFont(ImGuiIconFontText);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
	}

	static void Button_MD(const std::string& name,
						  const std::function<void()> function = []() -> void {return; },
						  const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushFont(ImGuiIconFontMd);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
	}

	static void Button_BG(const std::string& name,
						  const std::function<void()> function = []() -> void {return; },
						  const ImVec2& size = ImVec2(0, 0))
	{
		ImGui::PushFont(ImGuiIconFontBg);

		if (ImGui::Button(name.c_str(), size))
		{
			function();
		}

		ImGui::PopFont();
	}


}