#pragma once

#include <Opium.h>
#include <ParticleSystem.h>


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
	Opium::Ref<Opium::Texture2D> m_Spritesheet;
	Opium::Ref<Opium::SubTexture2D> m_TextureStairs;
	Opium::Ref<Opium::SubTexture2D> m_TextureBush;
	Opium::Ref<Opium::SubTexture2D> m_TextureTree;
	Opium::Ref<Opium::SubTexture2D> m_TextureBarrel;

	float m_TilingFactor = 1.0f;

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Opium::Ref<Opium::SubTexture2D>> m_TextureMap;
};