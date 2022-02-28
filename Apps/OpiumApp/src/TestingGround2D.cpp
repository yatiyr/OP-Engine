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
		Opium::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f, -0.1f }, { 30.0f, 30.0f }, 45.0f, m_CheckerboardTexture, m_TilingFactor, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		Opium::Renderer2D::EndScene();

		Opium::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -5.0f; y < 5.0f; y += 0.03f)
		{
			OP_PROFILE_SCOPE("TestingGround2D::Renderer Draw Loop");
			for (float x = -5.0f; x < 5.0f; x += 0.03f)
			{
				OP_PROFILE_SCOPE("TestingGround2D::Renderer Draw Loop Inner");
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Opium::Renderer2D::DrawQuad({ x * 20, y * 20 }, { 0.45f, 0.45f }, color);
			}
		}

		Opium::Renderer2D::EndScene();
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
