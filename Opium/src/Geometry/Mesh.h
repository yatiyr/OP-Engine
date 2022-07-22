#pragma once

#include <Renderer/VertexArray.h>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <vector>

namespace OP
{
	class Mesh
	{
	public:

		Mesh(bool smooth);
		Mesh();
		~Mesh();

		virtual void BuildVertices() = 0;


		virtual void SetupArrayBuffer();
		virtual void SetupMesh();

		virtual void SetupTangentBitangents();
		void AddIndices(uint32_t i1, uint32_t i2, uint32_t i3);

		void ClearArrays();


		uint32_t GetVertexCount()    const;
		uint32_t GetNormalCount()    const;
		uint32_t GetBitangentCount() const;
		uint32_t GetTangentCount()   const;
		uint32_t GetTexCoordCount()  const;
		uint32_t GetIndexCount()     const;
		uint32_t GetTriangleCount()  const;
		uint32_t GetVertexSize()     const;
		uint32_t GetNormalSize()     const;
		uint32_t GetTangentSize()    const;
		uint32_t GetBitangentSize()  const;
		uint32_t GetTexCoordSize()   const;
		uint32_t GetIndexSize()      const;

		// float GetRadius() const; Sphere;

		const std::vector<glm::vec3> GetVertices()   const;
		const std::vector<glm::vec3> GetNormals()    const;
		const std::vector<glm::vec3> GetTangents()   const;
		const std::vector<glm::vec3> GetBitangents() const;
		const std::vector<glm::vec2> GetTexCoords() const;

		const uint32_t* GetIndices() const;


		void Draw() const;
		void SetSmooth(bool smooth);

	protected:
		std::vector<glm::vec3> CalcTangentBitangents(glm::vec3 pos1,
			glm::vec3 pos2,
			glm::vec3 pos3,
			glm::vec2 uv1,
			glm::vec2 uv2,
			glm::vec2 uv3);

		glm::vec3 ComputeFaceNormal(const glm::vec3& v1,
			const glm::vec3& v2,
			const glm::vec3& v3);


		
		std::vector<glm::vec3> m_Vertices;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::vec3> m_Bitangents;
		std::vector<glm::vec2> m_TexCoords;


		// Array object and buffers

		std::vector<float> m_ArrayBuffer;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;


		bool m_Smooth;


	};
}