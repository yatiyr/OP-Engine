#pragma once

#include <Config/Precomp.h>
#include <Opium/Core.h>
#include <EventSystem/Event.h>


namespace Opium
{
	struct WinProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WinProperties(const std::string& title = "Opium 03",
					  uint32_t width = 1280,
			          uint32_t height = 720) : 
					  Title(title), Width(width), Height(height) {}
	};

	class OPIUM_API Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual float GetTime() const = 0;

		static Window* Create(const WinProperties& props = WinProperties());
	};
}