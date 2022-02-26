#pragma once

#include <Opium/Layer.h>

#include <EventSystem/AppEvent.h>
#include <EventSystem/KeyEvent.h>
#include <EventSystem/MouseEvent.h>

namespace Opium
{
	class OPIUM_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
		
	private:
		float m_Time = 0.0f;

	};
}