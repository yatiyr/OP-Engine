#pragma once

#include <Op/Layer.h>

#include <EventSystem/AppEvent.h>
#include <EventSystem/KeyEvent.h>
#include <EventSystem/MouseEvent.h>

#include <unordered_map>


namespace OP
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
		
		void BlockEvents(bool block) { m_BlockEvents = block; }

		void* GetFontPtr(const std::string& fontName);

		void SetDarkThemeColors();
		// void PushGlobalStyles();
		// void PopGlobalStyles();
		void SetGlobalStyles();
	private:

		void LoadFonts(const std::string& fontName);
		void LoadIconFonts();
		std::unordered_map<std::string, void*> ImGuiFontTable;

		bool m_BlockEvents = true;
		float m_Time = 0.0f;

	};
}