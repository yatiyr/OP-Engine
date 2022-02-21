#pragma once

#include <Config/Precomp.h>
#include <Opium/Core.h>
#include <EventSystem/Event.h>


namespace Opium
{
	struct WinProperties
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WinProperties(const std::string& title = "Opium 03",
					  unsigned int width = 1440,
			          unsigned int height = 800) : 
					  Title(title), Width(width), Height(height) {}
	};

	class OPIUM_API Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}
		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WinProperties& props = WinProperties());
	};
}