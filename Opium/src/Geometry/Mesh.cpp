#include <Precomp.h>
#include <Geometry/Mesh.h>

// Temp
#include <Renderer/RenderCommand.h>

namespace OP
{
	Mesh::Mesh(bool smooth) : m_Smooth(smooth) {}
	Mesh::Mesh() {}

	Mesh::~Mesh()
	{
	}

	void Mesh::SetupArrayBuffer()
	{
		m_ArrayBuffer.clear();
		

		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			// Add position
			m_ArrayBuffer.push_back(m_Vertices[i].x);
			m_ArrayBuffer.push_back(m_Vertices[i].y);
			m_ArrayBuffer.push_back(m_Vertices[i].z);

			// Add Normal
			m_ArrayBuffer.push_back(m_Normals[i].x);
			m_ArrayBuffer.push_back(m_Normals[i].y);
			m_ArrayBuffer.push_back(m_Normals[i].z);

			// Add Texture Coords
			m_ArrayBuffer.push_back(m_TexCoords[i].x);
			m_ArrayBuffer.push_back(m_TexCoords[i].y);

			// Add Tangents
			m_ArrayBuffer.push_back(m_Tangents[i].x);
			m_ArrayBuffer.push_back(m_Tangents[i].y);
			m_ArrayBuffer.push_back(m_Tangents[i].z);

			// Add Bitangents
			m_ArrayBuffer.push_back(m_Bitangents[i].x);
			m_ArrayBuffer.push_back(m_Bitangents[i].y);
			m_ArrayBuffer.push_back(m_Bitangents[i].z);

		}

	}

	void Mesh::SetupMesh()
	{
		// clear buffers if they are already allocated

		if (m_VertexArray.get())
			delete m_VertexArray.get();

		
		m_VertexArray = VertexArray::Create();
		
		m_VertexBuffer = VertexBuffer::Create(&m_ArrayBuffer[0], m_ArrayBuffer.size() * sizeof(float));
		m_VertexBuffer->SetLayout(
			{
				{ ShaderDataType::Float3, "a_Position"},
				{ ShaderDataType::Float3, "a_Normal"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float3, "a_Tangent"},
				{ ShaderDataType::Float3, "a_Bitangent"}
			}
		);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_IndexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::SetupTangentBitangents()
	{
		// initialize tangent and bitangent
		m_Tangents = std::vector<glm::vec3>(m_Normals.size(), glm::vec3(0.0f));
		m_Bitangents = std::vector<glm::vec3>(m_Normals.size(), glm::vec3(0.0f));

		// initialize vectors for vertices and texture coords
		glm::vec3 v1, v2, v3;
		glm::vec2 t1, t2, t3;

		// traverse all triangles and calculate tangents and bitangents
		// for all vertices
		for (uint32_t i = 0; i < m_Indices.size(); i += 3)
		{
			v1 = m_Vertices[m_Indices[i]];
			v2 = m_Vertices[m_Indices[i+1]];
			v3 = m_Vertices[m_Indices[i+2]];

			t1 = m_TexCoords[m_Indices[i]];
			t2 = m_TexCoords[m_Indices[i+1]];
			t3 = m_TexCoords[m_Indices[i+2]];

			// Get Tangent and Bitangent Vectors
			std::vector<glm::vec3> TB = CalcTangentBitangents(v1, v2, v3,
															  t1, t2, t3);

			m_Tangents[m_Indices[i]]     += TB[0];
			m_Bitangents[m_Indices[i]]   += TB[1];

			m_Tangents[m_Indices[i+1]]   += TB[0];
			m_Bitangents[m_Indices[i+1]] += TB[1];

			m_Tangents[m_Indices[i+2]]   += TB[0];
			m_Bitangents[m_Indices[i+2]] += TB[1];
		}

		// orthogonalize and normalize tangents and bitangents
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			glm::vec3& normal = m_Normals[i];
			glm::vec3& tangent = m_Tangents[i];
			glm::vec3& bitangent = m_Bitangents[i];


			normal = glm::normalize(normal);
			tangent = glm::normalize(tangent);
			bitangent = glm::normalize(bitangent);

			// gram-schmidt orthogonalize
			tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));

			// calculate handedness
			if (glm::dot(glm::cross(tangent, normal), bitangent) < 0.0f)
				tangent *= -1.0f;

			bitangent = glm::normalize(glm::cross(tangent, normal));
		}


	}

	void Mesh::AddIndices(uint32_t i1, uint32_t i2, uint32_t i3)
	{
		m_Indices.push_back(i1);
		m_Indices.push_back(i2);
		m_Indices.push_back(i3);
	}

	void Mesh::ClearArrays()
	{
		m_Vertices.clear();
		m_Normals.clear();
		m_Tangents.clear();
		m_Bitangents.clear();
		m_TexCoords.clear();
		m_Indices.clear();
	}

	uint32_t Mesh::GetVertexCount()   const { return (unsigned int)m_Vertices.size() / 3; }
	uint32_t Mesh::GetNormalCount()   const { return (unsigned int)m_Normals.size() / 3; }
	uint32_t Mesh::GetBitangentCount() const{ return (unsigned int)m_Bitangents.size() / 3; }
	uint32_t Mesh::GetTangentCount()  const { return (unsigned int)m_Tangents.size() / 3; }
	uint32_t Mesh::GetTexCoordCount() const { return (unsigned int)m_TexCoords.size() / 2; }
	uint32_t Mesh::GetIndexCount()    const { return (unsigned int)m_Indices.size(); }
	uint32_t Mesh::GetTriangleCount() const { return GetIndexCount() / 3; }
	uint32_t Mesh::GetVertexSize()    const { return (unsigned int)m_Vertices.size() * sizeof(float); }
	uint32_t Mesh::GetNormalSize()    const { return (unsigned int)m_Normals.size() * sizeof(float); }
	uint32_t Mesh::GetTangentSize()   const { return (unsigned int)m_Tangents.size() * sizeof(float); }
	uint32_t Mesh::GetBitangentSize() const { return (unsigned int)m_Bitangents.size() * sizeof(float); }
	uint32_t Mesh::GetTexCoordSize()  const { return (unsigned int)m_TexCoords.size() * sizeof(float); }
	uint32_t Mesh::GetIndexSize()     const { return (unsigned int)m_Indices.size() * sizeof(unsigned int); }

	const std::vector<glm::vec3> Mesh::GetVertices()   const { return m_Vertices; }
	const std::vector<glm::vec3> Mesh::GetNormals()    const { return m_Normals; }
	const std::vector<glm::vec3> Mesh::GetTangents()   const { return m_Tangents; }
	const std::vector<glm::vec3> Mesh::GetBitangents() const { return m_Bitangents; }
	const std::vector<glm::vec2> Mesh::GetTexCoords()  const { return m_TexCoords; }

	const uint32_t* Mesh::GetIndices() const { return m_Indices.data(); }

	void Mesh::Draw() const
	{
		RenderCommand::DrawIndexedBinded(m_VertexArray, m_Indices.size());
	}

	void Mesh::SetSmooth(bool smooth)
	{
		this->m_Smooth = smooth;
	}

	std::vector<glm::vec3> Mesh::CalcTangentBitangents(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
		                                               glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
	{
		std::vector<glm::vec3> result;

		glm::vec3 tangent, bitangent;

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;

		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		result.push_back(tangent);
		result.push_back(bitangent);

		return result;
	}

	glm::vec3 Mesh::ComputeFaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{

		const float EPSILON = 0.000001f;

		glm::vec3 normal(0.0f);


		glm::vec3 e1 = v2 - v1;
		glm::vec3 e2 = v3 - v1;

		normal = glm::cross(e1, e2);
		if (glm::length(normal) > EPSILON)
		{
			normal = glm::normalize(normal);
			return normal;
		}


		return glm::vec3(0.0f);
	}

}