#include <Precomp.h>
#include "Geometry/Cube.h"

namespace OP
{
	Cube::Cube()
	{
		BuildVertices();
	}

	Ref<Cube> Cube::Create()
	{
		return std::make_shared<Cube>();
	}


	void Cube::BuildVertices()
	{


		// Clear arrays for safety
		ClearArrays();

		// Fill in vertices, normals and indices
		
		// Front - face
		m_Vertices .push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(0.5f, -0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(0.5f, 0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));


		// Back - face
		m_Vertices .push_back(glm::vec3(0.5f, 0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices .push_back(glm::vec3(0.5f, -0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));


		// Left - face
		m_Vertices .push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));


		// Right - face
		m_Vertices .push_back(glm::vec3(0.5f, 0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices .push_back(glm::vec3(0.5f, -0.5f, 0.5f));
		m_Normals  .push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(0.5f, -0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices .push_back(glm::vec3(0.5f, 0.5f, -0.5f));
		m_Normals  .push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));

		// Top - face
		m_Vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));

		// Bottom - face
		m_Vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));


		SetupTangentBitangents();
		SetupArrayBuffer();
		SetupMesh();
	}
}
