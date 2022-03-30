#pragma once

#include <Opium/Core.h>
#include <Precomp.h>

namespace OP
{
	enum class EventType
	{
		None = 0,
		// ----- WINDOW EVENTS -----
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		// ------ APP EVENTS ------
		AppTick,
		AppUpdate,
		AppRender,
		// ------ KEY EVENTS ------
		KeyPressed,
		KeyReleased,
		KeyTyped,
		// ----- MOUSE EVENTS -----
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BITFIELD(0),
		EventCategoryInput = BITFIELD(1),
		EventCategoryKeyboard = BITFIELD(2),
		EventCategoryMouse = BITFIELD(3),
		EventCategoryMouseButton = BITFIELD(4)
	};


#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
							   virtual EventType GetEventType() const override { return GetStaticType(); } \
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
		friend class EventDispatcher;
	public:
		bool Handled = false;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};



	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& e) :m_Event(e) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(*(T*)&m_Event);
				return m_Event.Handled;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}