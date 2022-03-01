#include <TestingGround2D.h>
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

TestingGround2D::TestingGround2D()
	: Layer("TestingGround2D"), m_CameraController(1280.0f / 720.0f, true)
{

}

void TestingGround2D::OnAttach()
{
	OP_PROFILE_FUNCTION();

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109.0f / 255.0f , 41.0f / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CheckerboardTexture = Opium::Texture2D::Create("assets/textures/Checkerboard.png");

}

void TestingGround2D::OnDetach()
{
	OP_PROFILE_FUNCTION();
}

void TestingGround2D::OnUpdate(Opium::Timestep ts)
{
	OP_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Opium::Renderer2D::ResetStats();
	{
		OP_PROFILE_SCOPE("TestingGround2D::Renderer Prep");
		Opium::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Opium::RenderCommand::Clear();
	}

	{
		OP_PROFILE_SCOPE("TestingGround2D::Renderer Draw");
		Opium::Renderer2D::BeginScene(m_CameraController.GetCamera());
		// Opium::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Opium::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Opium::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.7f, 1.0f });
		Opium::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, -0.1f }, { 30.0f, 30.0f }, glm::radians(45.0f), m_CheckerboardTexture, m_TilingFactor, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		Opium::Renderer2D::EndScene();

		Opium::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.1f)
		{
			OP_PROFILE_SCOPE("TestingGround2D::Renderer Draw Loop");
			for (float x = -5.0f; x < 5.0f; x += 0.1f)
			{
				OP_PROFILE_SCOPE("TestingGround2D::Renderer Draw Loop Inner");
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Opium::Renderer2D::DrawQuad( { x , y }, { 0.45f, 0.45f }, color);
			}
		}

		Opium::Renderer2D::EndScene();

		if (Opium::Input::IsMouseButtonPressed(OP_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Opium::Input::GetMousePos();
			auto width = Opium::Application::Get().GetWindow().GetWidth();
			auto height = Opium::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 50; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	}


	// std::dynamic_pointer_cast<Opium::OpenGLShader>(m_FlatColorShader)->Bind();
	// std::dynamic_pointer_cast<Opium::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
}

void TestingGround2D::OnImGuiRender()
{

	OP_PROFILE_FUNCTION();

	auto stats = Opium::Renderer2D::GetStats();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::DragFloat("Tiling Factor", &m_TilingFactor);
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d:", stats.DrawCalls);
	ImGui::Text("QuadCount: %d:", stats.QuadCount);
	ImGui::Text("Vertices: %d:", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d:", stats.GetTotalIndexCount());
	ImGui::End();
}

void TestingGround2D::OnEvent(Opium::Event& e)
{
	m_CameraController.OnEvent(e);
}
