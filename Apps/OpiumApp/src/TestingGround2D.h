#pragma once

#include <Opium.h>

class TestingGround2D : public Opium::Layer 
{
public:
	TestingGround2D();
	virtual ~TestingGround2D() {}
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Opium::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Opium::Event& e) override;
private:
	Opium::OrthographicCameraController m_CameraController;

	// Temporary
	Opium::Ref<Opium::VertexArray> m_SquareVA;
	Opium::Ref<Opium::Shader> m_FlatColorShader;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	Opium::Ref<Opium::Texture2D> m_CheckerboardTexture;
};