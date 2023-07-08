#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Window/Window.h>
#include <Renderer/GraphicsContext.h>

namespace OP
{
	class WinWindow : public Window
	{
	public:
		WinWindow(const WinProperties& props);
		virtual ~WinWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		bool IsVSync() const override;
		void SetVSync(bool enabled) override;
		inline void SetEventCallback(const EventCallbackFunction& callback) override { m_Data.EventCallback = callback; }


		virtual float GetTime() const override;
		inline virtual void* GetNativeWindow() const { return m_Window; }

		virtual float GetDpiScale() const override;

		virtual void HideMouseCursor() const override;
		virtual void ShowMouseCursor() const override;
	private:
		virtual void Init(const WinProperties& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WinData
		{
			std::string Title;
			unsigned int Width = 1280, Height = 720;
			bool VSync = false;

			EventCallbackFunction EventCallback;
		};

		WinData m_Data;
	};
}