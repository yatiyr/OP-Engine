#include <Precomp.h>
#include "Capsule.h"


namespace OP
{

	const int MIN_SECTOR_COUNT_Capsule = 3;
	const int MIN_STACK_COUNT_Capsule = 1;
	const int MIN_HEMISPHERE_STACK_COUNT_Capsule = 4;

	Capsule::Capsule(float baseRadius, float topRadius, float height, int sectorCount, int hemisphereStackCount, int stackCount, bool smooth)
	{
		Set(baseRadius, topRadius, height, sectorCount, hemisphereStackCount, stackCount, smooth);
	}

	void Capsule::Set(float baseRadius, float topRadius, float height, int sectorCount, int hemisphereStackCount, int stackCount, bool smooth)
	{
		this->m_BaseRadius = baseRadius;
		this->m_TopRadius = topRadius;
		this->m_Height = height;
		this->m_SectorCount = sectorCount;
		if (sectorCount < MIN_SECTOR_COUNT_Capsule)
			this->m_SectorCount = MIN_SECTOR_COUNT_Capsule;
		this->m_HemisphereStackCount = hemisphereStackCount;
		if (hemisphereStackCount < MIN_HEMISPHERE_STACK_COUNT_Capsule)
			this->m_HemisphereStackCount = hemisphereStackCount;
		this->m_StackCount = stackCount;
		if (stackCount < MIN_STACK_COUNT_Capsule)
			this->m_StackCount = MIN_STACK_COUNT_Capsule;
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
	void Capsule::GenerateUnitCircleVertices()
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

	void Capsule::BuildVerticesSmooth()
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
		uint32_t sphereTopIndex = (uint32_t)m_Vertices.size();
		const float PI = acos(-1);                  // position of vertex

		float sectorStep = 2 * PI / m_SectorCount;
		float stackStep = PI / ( 2 * m_HemisphereStackCount);
		float sectorAngle, stackAngle;

		for (int i = 0; i <= m_HemisphereStackCount; i++)
		{
			float xz;
			glm::vec3 vertex(0.0f, 0.0f, 0.0f);
			glm::vec2 texCoord(0.0f, 0.0f);

			// stack angle is fi
			stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to 0

			xz = m_TopRadius * cosf(stackAngle);
			vertex.y = m_TopRadius * sinf(stackAngle) + 0.5 * m_Height;
			

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= m_SectorCount; j++)
			{

				sectorAngle = j * sectorStep;

				// vertex position
				vertex.z = xz * cosf(sectorAngle);
				vertex.x = xz * sinf(sectorAngle);
				m_Vertices.push_back(vertex);

				// normalized vertex normal
				glm::vec3 norm(0.0f);
				norm.z = cosf(stackAngle) * cosf(sectorAngle);
				norm.x = cosf(stackAngle) * sinf(sectorAngle);
				norm.y = sinf(stackAngle);
				norm = glm::normalize(norm);
				m_Normals.push_back(norm);

				// vertex tex coord between [0, 1]
				texCoord.x = (float)j / m_SectorCount;
				texCoord.y = (float)i / (m_HemisphereStackCount);
				m_TexCoords.push_back(texCoord);
			}
		}


		uint32_t sphereBaseIndex = (uint32_t)m_Vertices.size();
		for (int i = 0; i <= m_HemisphereStackCount; i++)
		{
			float xz;
			glm::vec3 vertex(0.0f, 0.0f, 0.0f);
			glm::vec2 texCoord(0.0f, 0.0f);

			// stack angle is fi
			stackAngle = 0 - i * stackStep; // starting from 0 to -pi/2

			xz = m_BaseRadius * cosf(stackAngle);
			vertex.y = m_BaseRadius * sinf(stackAngle) - 0.5 * m_Height;


			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= m_SectorCount; j++)
			{

				sectorAngle = j * sectorStep;

				// vertex position
				vertex.z = xz * cosf(sectorAngle);
				vertex.x = xz * sinf(sectorAngle);
				m_Vertices.push_back(vertex);

				// normalized vertex normal
				glm::vec3 norm(0.0f);
				norm.z = cosf(stackAngle) * cosf(sectorAngle);
				norm.x = cosf(stackAngle) * sinf(sectorAngle);
				norm.y = sinf(stackAngle);
				norm = glm::normalize(norm);
				m_Normals.push_back(norm);

				// vertex tex coord between [0, 1]
				texCoord.x = (float)j / m_SectorCount;
				texCoord.y = (float)i / (m_HemisphereStackCount);
				m_TexCoords.push_back(texCoord);
			}
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

		// hemisphere indices

		uint32_t topHemisphereIndices = (uint32_t)m_Indices.size();
		// indices
		//  k1--k1+1
		//  |  / |
		//  | /  |
		//  k2--k2+1
		for (int i = 0; i < m_HemisphereStackCount; i++)
		{
			k1 = i * (m_SectorCount + 1) + sphereTopIndex;
			k2 = k1 + m_SectorCount + 1;

			for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
			{
				if (i != 0)
					AddIndices(k1, k2, k1 + 1);
				AddIndices(k1 + 1, k2, k2 + 1);
			}
		}

		uint32_t baseHemisphereIndices = (uint32_t)m_Indices.size();
		// indices
		//  k1--k1+1
		//  |  / |
		//  | /  |
		//  k2--k2+1
		for (int i = 0; i < m_HemisphereStackCount; i++)
		{
			k1 = i * (m_SectorCount + 1) + sphereBaseIndex;
			k2 = k1 + m_SectorCount + 1;

			for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
			{
				AddIndices(k1, k2, k1 + 1);
				if (i != (m_HemisphereStackCount - 1))
					AddIndices(k1 + 1, k2, k2 + 1);
			}
		}

	}

	std::vector<float> Capsule::GetSideNormals()
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

	void Capsule::SetHemispheres(uint32_t baseIndex)
	{
		const float PI = acos(-1);

		// tmp vertex definition
		struct Vertex
		{
			float x, y, z, s, t;
		};


		float sectorStep = 2 * PI / m_SectorCount;
		float stackStep = PI / (m_HemisphereStackCount * 2);
		float sectorAngle, stackAngle;

		std::vector<Vertex> tmpVerticesTop;
		std::vector<Vertex> tmpVerticesBase;

		// compute all vertices first without computing normals
		for (int i = 0; i <= m_HemisphereStackCount; i++)
		{
			stackAngle = PI / 2 - i * stackStep;      // starting from pi/2 to 0
			float xz;
			float y;

			xz = m_TopRadius * cosf(stackAngle);   // r * cos(u)
			y = m_TopRadius * sinf(stackAngle);   // r * sin(u)



			// add (m_SectorCount + 1) vertices per stack
			// the first and last vertices have same position and normal
			// but different tex coords
			for (int j = 0; j <= m_SectorCount; j++)
			{
				sectorAngle = j * sectorStep;         // starting from 0 to 2pi

				Vertex vertex;
				vertex.x = xz * sinf(sectorAngle);    // x = r * cos(u) * sin(v)
				vertex.y = y;                         // y = r * sin(u)
				vertex.z = xz * cosf(sectorAngle);    // z = r * cos(u) * cos(v)
				vertex.s = (float)j / m_SectorCount;    // s
				vertex.t = (float)i / m_HemisphereStackCount;      // t
				tmpVerticesTop.push_back(vertex);
			}
		}

		for (int i = 0; i <= m_HemisphereStackCount; i++)
		{
			stackAngle = 0 - i * stackStep; // 0 to -pi/2
			float xz;
			float y;

			xz = m_BaseRadius * cosf(stackAngle);
			y = m_BaseRadius * sinf(stackAngle);

			// add (m_SectorCount + 1) vertices per stack
			// the first and last vertices have same position and normal
			// but different tex coords
			for (int j = 0; j <= m_SectorCount; j++)
			{
				sectorAngle = j * sectorStep;         // starting from 0 to 2pi

				Vertex vertex;
				vertex.x = xz * sinf(sectorAngle);    // x = r * cos(u) * sin(v)
				vertex.y = y;                         // y = r * sin(u)
				vertex.z = xz * cosf(sectorAngle);    // z = r * cos(u) * cos(v)
				vertex.s = (float)j / m_SectorCount;    // s
				vertex.t = (float)i / m_HemisphereStackCount;      // t
				tmpVerticesBase.push_back(vertex);
			}
		}


		Vertex v1, v2, v3, v4;                   // 4 vertex positions and tex coords
		glm::vec3 n;                             // 1 face normal

		int i, j, k, vi1, vi2;
		int topIndex = baseIndex;                           // index for vertex
		for (i = 0; i < m_HemisphereStackCount; i++)
		{
			vi1 = i * (m_SectorCount + 1);         // index of tmpVertices
			vi2 = (i + 1) * (m_SectorCount + 1);

			for (j = 0; j < m_SectorCount; j++, vi1++, vi2++)
			{
				// get 4 vertices per sector
				// v1--v3
				// |    |
				// v2--v4
				v1 = tmpVerticesTop[vi1];
				v2 = tmpVerticesTop[vi2];
				v3 = tmpVerticesTop[vi1 + 1];
				v4 = tmpVerticesTop[vi2 + 1];

				// if 1st stack and last stack, store only 1 triangle per sector
				// otherwise, store 2 triangles (quad) per sector
				if (i == 0) // a triangle for first stack -------------------------
				{
					// add vertices of triangle
					m_Vertices.push_back(glm::vec3(v1.x, v1.y + m_Height * 0.5, v1.z));
					m_Vertices.push_back(glm::vec3(v2.x, v2.y + m_Height * 0.5, v2.z));
					m_Vertices.push_back(glm::vec3(v4.x, v4.y + m_Height * 0.5, v4.z));

					// put tex coords of triangle
					m_TexCoords.push_back(glm::vec2(v1.s, v1.t));
					m_TexCoords.push_back(glm::vec2(v2.s, v2.t));
					m_TexCoords.push_back(glm::vec2(v4.s, v4.t));

					// put normal
					n = ComputeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v4.x, v4.y, v4.z));
					for (k = 0; k < 3; k++) // same normal for three vertices
					{
						m_Normals.push_back(n);
					}

					// put indices of 1 triangle
					AddIndices(topIndex, topIndex + 1, topIndex + 2);

					topIndex += 3; // get next index
				}
				else // 2 triangles for others ----------------------------------
				{
					// put quad vertices: v1-v2-v3-v4
					m_Vertices.push_back(glm::vec3(v1.x, v1.y + m_Height * 0.5, v1.z));
					m_Vertices.push_back(glm::vec3(v2.x, v2.y + m_Height * 0.5, v2.z));
					m_Vertices.push_back(glm::vec3(v3.x, v3.y + m_Height * 0.5, v3.z));
					m_Vertices.push_back(glm::vec3(v4.x, v4.y + m_Height * 0.5, v4.z));

					// put tex coords of quad
					m_TexCoords.push_back(glm::vec2(v1.s, v1.t));
					m_TexCoords.push_back(glm::vec2(v2.s, v2.t));
					m_TexCoords.push_back(glm::vec2(v3.s, v3.t));
					m_TexCoords.push_back(glm::vec2(v4.s, v4.t));


					// put normal
					n = ComputeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v3.x, v3.y, v3.z));
					for (k = 0; k < 4; k++) // same normal for four vertices
					{
						m_Normals.push_back(n);
					}

					// put indices of quad (2 triangles)
					//          v1-v2-v3 and v3-v2-v4
					// v1--v3
					// |    |
					// v2--v4                
					AddIndices(topIndex, topIndex + 1, topIndex + 2);
					AddIndices(topIndex + 2, topIndex + 1, topIndex + 3);

					topIndex += 4;
				}
			}
		}

		uint32_t baseHemisphereIndex = (uint32_t) m_Vertices.size();
		for (i = 0; i < m_HemisphereStackCount; i++)
		{
			vi1 = i * (m_SectorCount + 1);         // index of tmpVertices
			vi2 = (i + 1) * (m_SectorCount + 1);

			for (j = 0; j < m_SectorCount; j++, vi1++, vi2++)
			{
				// get 4 vertices per sector
				// v1--v3
				// |    |
				// v2--v4
				v1 = tmpVerticesBase[vi1];
				v2 = tmpVerticesBase[vi2];
				v3 = tmpVerticesBase[vi1 + 1];
				v4 = tmpVerticesBase[vi2 + 1];


				if (i == (m_HemisphereStackCount - 1)) // a triangle for last stack --------
				{
					// put a triangle
					m_Vertices.push_back(glm::vec3(v1.x, v1.y - m_Height * 0.5, v1.z));
					m_Vertices.push_back(glm::vec3(v2.x, v2.y - m_Height * 0.5, v2.z));
					m_Vertices.push_back(glm::vec3(v3.x, v3.y - m_Height * 0.5, v3.z));

					// put tex coords of triangle
					m_TexCoords.push_back(glm::vec2(v1.s, v1.t));
					m_TexCoords.push_back(glm::vec2(v2.s, v2.t));
					m_TexCoords.push_back(glm::vec2(v3.s, v3.t));

					// put normal
					n = ComputeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v3.x, v3.y, v3.z));
					for (k = 0; k < 3; k++) // same normal for three vertices
					{
						m_Normals.push_back(n);
					}

					// put indices of 1 triangle
					AddIndices(baseHemisphereIndex, baseHemisphereIndex + 1, baseHemisphereIndex + 2);

					baseHemisphereIndex += 3; // get next index
				}
				else // 2 triangles for others ----------------------------------
				{
					// put quad vertices: v1-v2-v3-v4
					m_Vertices.push_back(glm::vec3(v1.x, v1.y - m_Height * 0.5, v1.z));
					m_Vertices.push_back(glm::vec3(v2.x, v2.y - m_Height * 0.5, v2.z));
					m_Vertices.push_back(glm::vec3(v3.x, v3.y - m_Height * 0.5, v3.z));
					m_Vertices.push_back(glm::vec3(v4.x, v4.y - m_Height * 0.5, v4.z));

					// put tex coords of quad
					m_TexCoords.push_back(glm::vec2(v1.s, v1.t));
					m_TexCoords.push_back(glm::vec2(v2.s, v2.t));
					m_TexCoords.push_back(glm::vec2(v3.s, v3.t));
					m_TexCoords.push_back(glm::vec2(v4.s, v4.t));


					// put normal
					n = ComputeFaceNormal(glm::vec3(v1.x, v1.y, v1.z), glm::vec3(v2.x, v2.y, v2.z), glm::vec3(v3.x, v3.y, v3.z));
					for (k = 0; k < 4; k++) // same normal for four vertices
					{
						m_Normals.push_back(n);
					}

					// put indices of quad (2 triangles)
					//          v1-v2-v3 and v3-v2-v4
					// v1--v3
					// |    |
					// v2--v4                
					AddIndices(baseHemisphereIndex, baseHemisphereIndex + 1, baseHemisphereIndex + 2);
					AddIndices(baseHemisphereIndex + 2, baseHemisphereIndex + 1, baseHemisphereIndex + 3);

					baseHemisphereIndex += 4;
				}
			}
		}
	}

	void Capsule::BuildVerticesFlat()
	{
		struct Vertex
		{
			float x, y, z, s, t;
		};
		std::vector<Vertex> tmpVertices;

		int i, j, k; // indices
		float x, y, z, s, t, radius;

		// put tmp vertices of Capsule side to array by scaling unit circle
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


		tmpVertices.clear();
		// base part
		SetHemispheres((uint32_t)m_Vertices.size());

	}

	void Capsule::SetBaseRadius(float baseRadius)
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

	void Capsule::SetTopRadius(float topRadius)
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

	void Capsule::SetHeight(float height)
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

	void Capsule::SetSectorCount(int sectorCount)
	{

		this->m_SectorCount = sectorCount;
		if (sectorCount < MIN_SECTOR_COUNT_Capsule)
			this->m_SectorCount = MIN_SECTOR_COUNT_Capsule;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Capsule::SetStackCount(int stackCount)
	{

		this->m_StackCount = stackCount;
		if (stackCount < MIN_STACK_COUNT_Capsule)
			this->m_StackCount = MIN_STACK_COUNT_Capsule;



		if (m_Smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();

		SetupTangentBitangents(true);
		SetupArrayBuffer();
		SetupMesh();
	}

	void Capsule::SetSmooth(bool smooth)
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

	Ref<Capsule> Capsule::Create(float baseRadius, float topRadius, float height, int sectorCount, int hemisphereStackCount, int stackCount, bool smooth)
	{
		return std::make_shared<Capsule>(baseRadius, topRadius, height, sectorCount, hemisphereStackCount, stackCount, smooth);
	}

	void Capsule::BuildVertices()
	{
	}

}