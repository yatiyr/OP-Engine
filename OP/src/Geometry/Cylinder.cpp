#include <Precomp.h>
#include <Geometry/Cylinder.h>


namespace OP
{

	const int MIN_SECTOR_COUNT_CYLINDER = 3;
	const int MIN_STACK_COUNT_CYLINDER = 1;

	Cylinder::Cylinder(float baseRadius, float topRadius, float height, int sectorCount, int stackCount, bool smooth)
	{
		Set(baseRadius, topRadius, height, sectorCount, stackCount, smooth);
	}

	void Cylinder::Set(float baseRadius, float topRadius, float height, int sectorCount, int stackCount, bool smooth)
	{
		this->m_BaseRadius  = baseRadius;
		this->m_TopRadius   = topRadius;
		this->m_Height      = height;
		this->m_SectorCount = sectorCount;
		if (sectorCount < MIN_SECTOR_COUNT_CYLINDER)
			this->m_SectorCount = MIN_SECTOR_COUNT_CYLINDER;
		this->m_StackCount = stackCount;
		if (stackCount < MIN_STACK_COUNT_CYLINDER)
			this->m_StackCount = MIN_STACK_COUNT_CYLINDER;
		this->m_Smooth = smooth;

		GenerateUnitCircleVertices();

		if (smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	// We generate unit circles on XZ plane
	void Cylinder::GenerateUnitCircleVertices()
	{
		const float PI = acos(-1);
		float sectorStep = 2 * PI / m_SectorCount;
		float sectorAngle;

		m_UnitCircleVertices.clear();
		for (int i = 0; i <= m_SectorCount; i++)
		{
			sectorAngle = i * sectorStep;
			m_UnitCircleVertices.push_back(sin(sectorAngle));
			m_UnitCircleVertices.push_back(0);
			m_UnitCircleVertices.push_back(cos(sectorAngle));
		}
	}

	void Cylinder::BuildVerticesSmooth()
	{
		ClearArrays();

		float x, y, z;
		float radius;
		float currStack;
		std::vector<float> sideNormals = GetSideNormals();

		for (int i = 0; i <= m_StackCount; i++)
		{
			currStack = (float)i / m_StackCount;

			y = -(m_Height * 0.5f) + currStack * m_Height;
			radius = m_BaseRadius + currStack * (m_TopRadius - m_BaseRadius);
			float t = 1.0f - currStack;

			for (int j = 0, k = 0; j <= m_SectorCount; j++, k += 3)
			{
				glm::vec3 vertex;
				vertex.x = m_UnitCircleVertices[k] * radius;
				vertex.y = y;
				vertex.z = m_UnitCircleVertices[k + 2] * radius;

				glm::vec3 normal;
				normal.x = sideNormals[k];
				normal.y = sideNormals[k + 1];
				normal.z = sideNormals[k + 2];

				glm::vec2 texCoord;
				texCoord.x = (float)j / m_SectorCount;
				texCoord.y = t;
				m_Vertices.push_back(vertex);
				m_Normals.push_back(normal);
				m_TexCoords.push_back(texCoord);
			}
		}

		// top and base vertices
		uint32_t baseVertexIndex = (uint32_t)m_Vertices.size();

		// base cylinder vertices
		y = -m_Height * 0.5f;
		m_Vertices.push_back(glm::vec3(0.0f, y, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.5f, 0.5f));
		for (int i = 0, j = 0; i < m_SectorCount; i++, j += 3)
		{
			glm::vec3 vertex;
			vertex.x = m_UnitCircleVertices[j];
			vertex.y = y;
			vertex.z = m_UnitCircleVertices[j + 2];

			m_Vertices.push_back(glm::vec3(vertex.x * m_BaseRadius, vertex.y, vertex.z * m_BaseRadius));
			m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
			m_TexCoords.push_back(glm::vec2(-vertex.z + 0.5f + 0.5f, -vertex.x * 0.5f + 0.5f));
		}

		// top cylinder idnex
		uint32_t topVertexIndex = (uint32_t)m_Vertices.size();

		// top cylinder vertices
		y = m_Height * 0.5f;
		m_Vertices.push_back(glm::vec3(0.0f, y, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.5f, 0.5f));
		for (int i = 0, j = 0; i < m_SectorCount; i++, j += 3)
		{
			glm::vec3 vertex;
			vertex.x = m_UnitCircleVertices[j];
			vertex.y = y;
			vertex.z = m_UnitCircleVertices[j + 2];

			m_Vertices.push_back(glm::vec3(vertex.x * m_TopRadius, vertex.y, vertex.z * m_TopRadius));
			m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
			m_TexCoords.push_back(glm::vec2(vertex.z + 0.5f + 0.5f, -vertex.x * 0.5f + 0.5f));
		}

		// side indices
		uint32_t k1, k2;
		for (int i = 0; i < m_StackCount; i++)
		{
			k1 = i * (m_SectorCount + 1); // current stack beginning
			k2 = k1 + m_SectorCount + 1; // next stack beginning

			for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
			{
				AddIndices(k1, k1 + 1, k2);
				AddIndices(k2, k1 + 1, k2 + 1);
			}
		}

		// base indices
		uint32_t baseIndex = (uint32_t)m_Indices.size();

		// put indices for base
		for (int i = 0, k = baseVertexIndex + 1; i < m_SectorCount; i++, k++)
		{
			if (i < (m_SectorCount - 1))
				AddIndices(baseVertexIndex, k + 1, k);
			else
				AddIndices(baseVertexIndex, baseVertexIndex + 1, k);
		}

		// top indices
		uint32_t topIndex = (uint32_t)m_Indices.size();

		// put indices for top
		for (int i = 0, k = topVertexIndex + 1; i < m_SectorCount; i++, k++)
		{
			if (i < m_SectorCount - 1)
				AddIndices(topVertexIndex, k, k + 1);
			else
				AddIndices(topVertexIndex, k, topVertexIndex + 1);
		}

	}

	std::vector<float> Cylinder::GetSideNormals()
	{
		const float PI = acos(-1);
		float sectorStep = 2 * PI / m_SectorCount;
		float sectorAngle;

		float yAngle = atan2(m_BaseRadius - m_TopRadius, m_Height);
		float x0 = 0.0;
		float y0 = sin(yAngle);
		float z0 = cos(yAngle);

		std::vector<float> normals;

		for (int i = 0; i <= m_SectorCount; i++)
		{
			sectorAngle = i * sectorStep;
			normals.push_back(sin(sectorAngle) * z0 + cos(sectorAngle) * x0);
			normals.push_back(y0);
			normals.push_back(cos(sectorAngle) * z0 - sin(sectorAngle) * x0);
		}

		return normals;
	}

	void Cylinder::BuildVerticesFlat()
	{
		struct Vertex
		{
			float x, y, z, s, t;
		};
		std::vector<Vertex> tmpVertices;

		int i, j, k; // indices
		float x, y, z, s, t, radius;

		// put tmp vertices of cylinder side to array by scaling unit circle
		for (int i = 0; i <= m_StackCount; i++)
		{
			y = -(m_Height * 0.5f) + (float)i / m_StackCount * m_Height; // vertexPos y
			radius = m_BaseRadius + (float)i / m_StackCount * (m_TopRadius - m_BaseRadius); // lerp
			t = 1.0f - (float)i / m_StackCount;

			for (j = 0, k = 0; j <= m_SectorCount; j++, k += 3)
			{
				x = m_UnitCircleVertices[k];
				z = m_UnitCircleVertices[k + 2];
				s = (float)j / m_SectorCount;

				Vertex vertex;
				vertex.x = x * radius;
				vertex.y = y;
				vertex.z = z * radius;
				vertex.s = s;
				vertex.t = t;
				tmpVertices.push_back(vertex);
			}
		
		}

		ClearArrays();

		Vertex v1, v2, v3, v4; // 4 face positions
		glm::vec3 n; // 1 face normal
		int vi1, vi2; // indices
		int index = 0;

		// v2 - v4 <== stack at i+1
		// |  \ |
		// v1 - v3 <== stack at i
		for (int i = 0; i < m_StackCount; i++)
		{
			vi1 = i * (m_SectorCount + 1);
			vi2 = (i + 1) * (m_SectorCount + 1);

			for (int j = 0; j < m_SectorCount; j++, vi1++, vi2++)
			{
				v1 = tmpVertices[vi1];
				v2 = tmpVertices[vi2];
				v3 = tmpVertices[vi1 + 1];
				v4 = tmpVertices[vi2 + 1];

				n = ComputeFaceNormal(glm::vec3(v1.x, v1.y, v1.z),
									  glm::vec3(v3.x, v3.y, v3.z),
									  glm::vec3(v2.x, v2.y, v2.z));

				// put quad vertices: v1-v2-v3-v4
				m_Vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
				m_Vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
				m_Vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));
				m_Vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

				m_TexCoords.push_back(glm::vec2(v1.s, v1.t));
				m_TexCoords.push_back(glm::vec2(v2.s, v2.t));
				m_TexCoords.push_back(glm::vec2(v3.s, v3.t));
				m_TexCoords.push_back(glm::vec2(v4.s, v4.t));

				for (int k = 0; k < 4; k++)
				{
					m_Normals.push_back(n);
				}

				// add indices
				AddIndices(index, index + 2, index + 1);
				AddIndices(index + 1, index + 2, index + 3);

				index += 4;
			}
		}

		// base part
		uint32_t baseIndex = (uint32_t)m_Indices.size();
		uint32_t baseVertexIndex = (uint32_t)m_Vertices.size();

		// put vertices of base cylinder
		y = -m_Height * 0.5f;
		m_Vertices.push_back(glm::vec3(0.0f, y, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.5f, 0.5f));
		for (int i = 0, j = 0; i < m_SectorCount; i++, j += 3)
		{
			x = m_UnitCircleVertices[j];
			z = m_UnitCircleVertices[j + 2];
			m_Vertices.push_back(glm::vec3(x * m_BaseRadius, y, z * m_BaseRadius));
			m_Normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
			m_TexCoords.push_back(glm::vec2(-z * 0.5f + 0.5f, -x * 0.5 + 0.5f));
		}

		// put base indices
		for (int i = 0, k = baseVertexIndex + 1; i < m_SectorCount; i++, k++)
		{
			if (i < m_SectorCount - 1)
				AddIndices(baseVertexIndex, k + 1, k);
			else
				AddIndices(baseVertexIndex, baseVertexIndex + 1, k);
		}

		// top index
		uint32_t topIndex = (uint32_t)m_Indices.size();
		uint32_t topVertexIndex = (uint32_t)m_Vertices.size();

		// put vertices of top cylinder
		y = m_Height * 0.5f;
		m_Vertices.push_back(glm::vec3(0.0f, y, 0.0f));
		m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_TexCoords.push_back(glm::vec2(0.5f, 0.5f));
		for (int i = 0, j = 0; i < m_SectorCount; i++, j += 3)
		{
			x = m_UnitCircleVertices[j];
			z = m_UnitCircleVertices[j + 2];
			m_Vertices.push_back(glm::vec3(x * m_TopRadius, y, z * m_TopRadius));
			m_Normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
			m_TexCoords.push_back(glm::vec2(z * 0.5f + 0.5f, -x * 0.5 + 0.5f));
		}

		// put base indices
		for (int i = 0, k = topVertexIndex + 1; i < m_SectorCount; i++, k++)
		{
			if (i < m_SectorCount - 1)
				AddIndices(topVertexIndex, k, k + 1);
			else
				AddIndices(topVertexIndex, k, topVertexIndex + 1);
		}

	}

	void Cylinder::SetBaseRadius(float baseRadius)
	{
		this->m_BaseRadius = baseRadius;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Cylinder::SetTopRadius(float topRadius)
	{
		this->m_TopRadius = topRadius;

	

		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Cylinder::SetHeight(float height)
	{

		this->m_Height = height;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Cylinder::SetSectorCount(int sectorCount)
	{

		this->m_SectorCount = sectorCount;
		if (sectorCount < MIN_SECTOR_COUNT_CYLINDER)
			this->m_SectorCount = MIN_SECTOR_COUNT_CYLINDER;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Cylinder::SetStackCount(int stackCount)
	{

		this->m_StackCount = stackCount;
		if (stackCount < MIN_STACK_COUNT_CYLINDER)
			this->m_StackCount = MIN_STACK_COUNT_CYLINDER;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Cylinder::SetSmooth(bool smooth)
	{

		this->m_Smooth = smooth;


		if (smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	Ref<Cylinder> Cylinder::Create(float baseRadius, float topRadius, float height, int sectorCount, int stackCount, bool smooth)
	{
		return std::make_shared<Cylinder>(baseRadius, topRadius, height, sectorCount, stackCount, smooth);
	}

	void Cylinder::BuildVertices()
	{
	}

}