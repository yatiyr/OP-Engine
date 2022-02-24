#include <Opium.h>
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

#include <glm/gtc/type_ptr.hpp>

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
		Opium::Ref<Opium::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Opium::VertexBuffer::Create(vertices, sizeof(vertices)));

		Opium::BufferLayout layout = {
			{ Opium::ShaderDataType::Float3, "a_Position"},
			{ Opium::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0,1,2 };
		Opium::Ref<Opium::IndexBuffer> indexBuffer;
		indexBuffer.reset(Opium::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		m_SquareVA.reset(Opium::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Opium::Ref<Opium::VertexBuffer> squareVB;
		squareVB.reset(Opium::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout(
			{
				{ Opium::ShaderDataType::Float3, "a_Position"},
				{ Opium::ShaderDataType::Float2, "a_TexCoord"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
		Opium::Ref<Opium::IndexBuffer> squareIB;
		squareIB.reset(Opium::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;


			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;			

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;


			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
		
			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_BlueShader.reset(Opium::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));


		std::string textureShaderVertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
		
			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			
			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		m_TextureShader.reset(Opium::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

		std::dynamic_pointer_cast<Opium::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Opium::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
		m_Texture = Opium::Texture2D::Create("assets/textures/Checkerboard.png");
    }

    void OnUpdate(Opium::Timestep ts) override
    {

		//OP_APP_INFO("{0} is the delta time", ts.GetSeconds());
		float deltaTime = ts;

		if (Opium::Input::IsKeyPressed(OP_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * deltaTime;
		}
		else if (Opium::Input::IsKeyPressed(OP_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * deltaTime;
		}

		if (Opium::Input::IsKeyPressed(OP_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * deltaTime;
		}
		else if (Opium::Input::IsKeyPressed(OP_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * deltaTime;
		}


		if (Opium::Input::IsKeyPressed(OP_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * deltaTime;
		else if (Opium::Input::IsKeyPressed(OP_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

        Opium::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Opium::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        //m_Camera.SetPosition({ 0.5f, 0.5f, 0.0 });
        m_Camera.SetRotation(m_CameraRotation);

        Opium::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Opium::OpenGLShader>(m_BlueShader)->Bind();
		std::dynamic_pointer_cast<Opium::OpenGLShader>(m_BlueShader)->UploadUniformFloat3("u_Color", m_SquareColor);
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.12f, y * 0.12f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Opium::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
		}

		m_Texture->Bind(0);
		Opium::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
       // Opium::Renderer::Submit(m_Shader, m_VertexArray);

        Opium::Renderer::EndScene();
    }

    void OnEvent(Opium::Event& event) override
    {
    }


    virtual void OnImGuiRender() override
    {
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
    }
private:
    Opium::Ref<Opium::Shader> m_Shader;
    Opium::Ref<Opium::VertexArray> m_VertexArray;


    Opium::Ref<Opium::Shader> m_BlueShader, m_TextureShader;
    Opium::Ref<Opium::VertexArray> m_SquareVA;

	Opium::Ref<Opium::Texture2D> m_Texture;


    Opium::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 1.0f;

	float m_CameraRotationSpeed = 10.0f;
	float m_CameraRotation = 0.0;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
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