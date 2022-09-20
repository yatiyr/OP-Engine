#include <Precomp.h>
#include <Geometry/Cube.h>

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
		m_Vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 1.0f));

		m_Vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 0.0f));

		m_Vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(1.0f, 1.0f));

		// set indices by constructing triangles
		m_Indices.push_back(0); m_Indices.push_back(1); m_Indices.push_back(2);
		m_Indices.push_back(0); m_Indices.push_back(2); m_Indices.push_back(3);

		m_Indices.push_back(4); m_Indices.push_back(5); m_Indices.push_back(6);
		m_Indices.push_back(4); m_Indices.push_back(6); m_Indices.push_back(7);

		m_Indices.push_back(8); m_Indices.push_back(9); m_Indices.push_back(10);
		m_Indices.push_back(8); m_Indices.push_back(10); m_Indices.push_back(11);
		
		m_Indices.push_back(12); m_Indices.push_back(13); m_Indices.push_back(14);
		m_Indices.push_back(12); m_Indices.push_back(14); m_Indices.push_back(15);

		m_Indices.push_back(16); m_Indices.push_back(17); m_Indices.push_back(18);
		m_Indices.push_back(16); m_Indices.push_back(18); m_Indices.push_back(19);

		m_Indices.push_back(20); m_Indices.push_back(21); m_Indices.push_back(22);
		m_Indices.push_back(20); m_Indices.push_back(22); m_Indices.push_back(23);


		SetupTangentBitangents(false);
		SetupArrayBuffer();
		SetupMesh();
	}
}
