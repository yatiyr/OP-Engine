#include <TestingGround2D.h>
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWDDDDDWWWWWWWWW"
"WWWWWWWWWDDDDDDDWWWWWWWW"
"WWWWWWWWDDDDDDDDDWWWWWWW"
"WWWWWWWDDDDDDDDDDDWWWWWW"
"WWWWWWDDDDDDDDDDDDDWWWWW"
"WWWWWDDDDDDDDDDDDDDDWWWW"
"WWWWDDDDDDDDDDDDDDDDDWWW"
"WWWDDDDDDDDDDDDDDDDDDDWW"
"WWDDDDDDWDDDDDDDDDDDDDDW"
"WDDDDDDWWWWDDDDDDDDDDDDW"
"WDDDDDDWWCWDDDDDDDDDDDWW"
"WWWDDDDDWWDDDDDDDDDDDWWW"
"WWWWDDDDDDDDDDDDDDDDDWWW"
"WWWWWDDDDDDDDDDDDDDDWWWW"
"WWWWWDDDDDDDDDDDDDDWWWWW"
"WWWWWWDDDDDDDDDDDDWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWWWWDDDDDDDWWWWWWWWW"
"WWWWWWWWWDDDDDDWWWWWWWWW"
"WWWWWWWWWWDWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
;

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
	m_Spritesheet = Opium::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");
	m_TextureStairs = Opium::SubTexture2D::CreateFromCoords(m_Spritesheet, { 0, 11 }, { 128, 128 });
	m_TextureTree = Opium::SubTexture2D::CreateFromCoords(m_Spritesheet, { 2, 1 }, { 128, 128 }, { 1, 2 });
	m_TextureBarrel = Opium::SubTexture2D::CreateFromCoords(m_Spritesheet, { 8, 2 }, { 128, 128 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

	m_TextureMap['D'] = Opium::SubTexture2D::CreateFromCoords(m_Spritesheet, { 6, 11 }, { 128, 128 });
	m_TextureMap['W'] = Opium::SubTexture2D::CreateFromCoords(m_Spritesheet, { 11, 11 }, { 128, 128 });

	m_CameraController.SetZoomLevel(5.0f);
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
		/*Opium::Renderer2D::BeginScene(m_CameraController.GetCamera());
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
		*/

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

		Opium::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];

				Opium::Ref<Opium::SubTexture2D> texture;

				if (m_TextureMap.find(tileType) != m_TextureMap.end())
				{
					texture = m_TextureMap[tileType];
				}
				else
					texture = m_TextureBarrel;


				Opium::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.5f }, { 1.0f, 1.0f }, texture);
			}
		}

		// Opium::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_TextureStairs, m_TilingFactor, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		// Opium::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_TextureBush, m_TilingFactor, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		//Opium::Renderer2D::DrawQuad({  1.0f, 0.0f, 0.5f }, { 1.0f, 2.0f }, m_TextureTree, m_TilingFactor, glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
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
