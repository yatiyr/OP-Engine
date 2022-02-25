#include <TestingGround2D.h>
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

TestingGround2D::TestingGround2D()
	: Layer("TestingGround2D"), m_CameraController(1280.0f / 720.0f, true)
{

}

void TestingGround2D::OnAttach()
{

	m_SquareVA = Opium::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Opium::Ref<Opium::VertexBuffer> squareVB;
	squareVB = Opium::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout(
		{
			{ Opium::ShaderDataType::Float3, "a_Position"}
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
	Opium::Ref<Opium::IndexBuffer> squareIB;
	squareIB = Opium::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);



	m_FlatColorShader = Opium::Shader::Create("assets/shaders/FlatColorShader.glsl");
}

void TestingGround2D::OnDetach()
{
}

void TestingGround2D::OnUpdate(Opium::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Opium::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Opium::RenderCommand::Clear();

	Opium::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Opium::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Opium::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	Opium::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Opium::Renderer::EndScene();
}

void TestingGround2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void TestingGround2D::OnEvent(Opium::Event& e)
{
	m_CameraController.OnEvent(e);
}
