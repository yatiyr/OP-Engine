#include <Opium.h>
#include <imgui/imgui.h>

class ExampleLayer : public Opium::Layer
{
public:
    ExampleLayer() : Layer("Orneks"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) , m_CameraPosition(0.0f)
    {
		m_VertexArray.reset(Opium::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.3f, 1.0f
		};

		// add vertex buffer class
		std::shared_ptr<Opium::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Opium::VertexBuffer::Create(vertices, sizeof(vertices)));

		Opium::BufferLayout layout = {
			{ Opium::ShaderDataType::Float3, "a_Position"},
			{ Opium::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0,1,2 };
		std::shared_ptr<Opium::IndexBuffer> indexBuffer;
		indexBuffer.reset(Opium::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		m_SquareVA.reset(Opium::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<Opium::VertexBuffer> squareVB;
		squareVB.reset(Opium::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(
			{
				{ Opium::ShaderDataType::Float3, "a_Position"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
		std::shared_ptr<Opium::IndexBuffer> squareIB;
		squareIB.reset(Opium::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;


			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				// color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(Opium::Shader::Create(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;


			uniform mat4 u_ViewProjection;			
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(Opium::Shader::Create(blueShaderVertexSrc, blueShaderFragmentSrc));
    }

    void OnUpdate() override
    {

		if (Opium::Input::IsKeyPressed(OP_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (Opium::Input::IsKeyPressed(OP_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed;
		}

		if (Opium::Input::IsKeyPressed(OP_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		else if (Opium::Input::IsKeyPressed(OP_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}


		if (Opium::Input::IsKeyPressed(OP_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed;
		else if (Opium::Input::IsKeyPressed(OP_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed;

        Opium::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Opium::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        //m_Camera.SetPosition({ 0.5f, 0.5f, 0.0 });
        m_Camera.SetRotation(m_CameraRotation);

        Opium::Renderer::BeginScene(m_Camera);

        Opium::Renderer::Submit(m_BlueShader, m_SquareVA);

        Opium::Renderer::Submit(m_Shader, m_VertexArray);

        Opium::Renderer::EndScene();
    }

    void OnEvent(Opium::Event& event) override
    {
    }


    virtual void OnImGuiRender() override
    {

    }
private:
    std::shared_ptr<Opium::Shader> m_Shader;
    std::shared_ptr<Opium::VertexArray> m_VertexArray;


    std::shared_ptr<Opium::Shader> m_BlueShader;
    std::shared_ptr<Opium::VertexArray> m_SquareVA;


    Opium::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.1f;

	float m_CameraRotationSpeed = 0.1f;
	float m_CameraRotation = 0.0;
};

class OpiumApp : public Opium::Application
{
public:
    OpiumApp()
    {
        PushLayer(new ExampleLayer());
    }

    ~OpiumApp()
    {

    }
};

Opium::Application* Opium::CreateApplication()
{
    return new OpiumApp;
}