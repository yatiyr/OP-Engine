#include <Precomp.h>

#include <Geometry/RadialSphere.h>

namespace OP
{

	const int MIN_SECTOR_COUNT = 3;
	const int MIN_STACK_COUNT  = 2;

	RadialSphere::RadialSphere(float radius, int sectorCount, int stackCount, bool smooth) : Mesh(smooth)
	{
		Set(radius, sectorCount, stackCount);
	}

	RadialSphere::~RadialSphere() {}

	void RadialSphere::Set(float radius, int sectorCount, int stackCount)
	{
		this->m_Radius = radius;

		this->m_SectorCount = sectorCount;
		if (sectorCount < MIN_SECTOR_COUNT)
			this->m_SectorCount = MIN_SECTOR_COUNT;

		this->m_StackCount = stackCount;
		if (stackCount < MIN_STACK_COUNT)
			this->m_StackCount = MIN_STACK_COUNT;

		BuildVertices();
	}

	void RadialSphere::SetRadius(float radius)
	{
		if (radius != m_Radius)
			Set(radius, m_SectorCount, m_StackCount);
	}

	void RadialSphere::SetSectorCount(int sectorCount)
	{
		if (sectorCount != m_SectorCount)
			Set(m_Radius, sectorCount, m_StackCount);
	}

	void RadialSphere::SetStackCount(int stackCount)
	{
		if (stackCount != m_StackCount)
			Set(m_Radius, m_SectorCount, stackCount);
	}

	void RadialSphere::PrintSelf() const
	{
		std::cout << "====== Radial Sphere ======\n"
			<< "         Radius: " << m_Radius << "\n"
			<< "   Sector Count:"  << m_SectorCount << "\n"
			<< "    Stack Count:"  << m_StackCount << "\n"
			<< " Triangle Count:"  << GetTriangleCount() << "\n"
			<< "    Index Count:"  << GetIndexCount() << "\n"
			<< "   Vertex Count:"  << GetVertexCount() << "\n"
			<< "   Normal Count:"  << GetNormalCount() << "\n"
			<< "Bitangent Count:"  << GetBitangentCount() << "\n"
			<< "  Tangent Count:"  << GetTangentCount() << "\n"
			<< " TexCoord Count:"  << GetTexCoordCount() << std::endl;
	}

	void RadialSphere::BuildVertices()
	{
        if (m_Smooth)
            BuildVerticesSmooth();
        else
            BuildVerticesFlat();
	}

	void RadialSphere::BuildVerticesSmooth()
	{
        const float PI = acos(-1);

        // clear mem
        ClearArrays();

        float xz;                            // position of vertex

        float sectorStep = 2 * PI / m_SectorCount;
        float stackStep = PI / m_StackCount;
        float sectorAngle, stackAngle;

        glm::vec3 vertex(0.0f, 0.0f, 0.0f);
        glm::vec2 texCoord(0.0f, 0.0f);

        for (int i = 0; i <= m_StackCount; i++)
        {
            // stack angle is fi
            stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
            xz = m_Radius * cosf(stackAngle);
            vertex.y = m_Radius * sinf(stackAngle);

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
                vertex = glm::normalize(vertex);
                m_Normals.push_back(vertex);

                // vertex tex coord between [0, 1]
                texCoord.x = (float)j / m_SectorCount;
                texCoord.y = (float)i / m_StackCount;
                m_TexCoords.push_back(texCoord);
            }
        }

        // indices
        //  k1--k1+1
        //  |  / |
        //  | /  |
        //  k2--k2+1
        unsigned int k1, k2;
        for (int i = 0; i < m_StackCount; i++)
        {
            k1 = i * (m_SectorCount + 1); // beginning of current stack
            k2 = k1 + m_SectorCount + 1;  // beginning of next stack

            for (int j = 0; j < m_SectorCount; j++, k1++, k2++)
            {
                // 2 triangles per sector excluding 1st and last stacks
                if (i != 0)
                {
                    AddIndices(k1, k2, k1 + 1);
                }
                if (i != (m_StackCount - 1))
                {
                    AddIndices(k1 + 1, k2, k2 + 1);
                }
            }
        }

        SetupTangentBitangents();
        SetupArrayBuffer();
        SetupMesh();
	}

	void RadialSphere::BuildVerticesFlat()
	{
        const float PI = acos(-1);

        // tmp vertex definition
        struct Vertex
        {
            float x, y, z, s, t;
        };

        std::vector<Vertex> tmpVertices;

        float sectorStep = 2 * PI / m_SectorCount;
        float stackStep = PI / m_StackCount;
        float sectorAngle, stackAngle;

        // compute all vertices first without computing normals
        for (int i = 0; i <= m_StackCount; i++)
        {
            stackAngle = PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
            float xz = m_Radius * cosf(stackAngle);   // r * cos(u)
            float y = m_Radius * sinf(stackAngle);   // r * sin(u)

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
                vertex.t = (float)i / m_StackCount;      // t
                tmpVertices.push_back(vertex);
            }
        }

        // clear memory
        ClearArrays();

        Vertex v1, v2, v3, v4;                   // 4 vertex positions and tex coords
        glm::vec3 n;                             // 1 face normal

        int i, j, k, vi1, vi2;
        int index = 0;                           // index for vertex
        for (i = 0; i < m_StackCount; i++)
        {
            vi1 = i * (m_SectorCount + 1);         // index of tmpVertices
            vi2 = (i + 1) * (m_SectorCount + 1);

            for (j = 0; j < m_SectorCount; j++, vi1++, vi2++)
            {
                // get 4 vertices per sector
                // v1--v3
                // |    |
                // v2--v4
                v1 = tmpVertices[vi1];
                v2 = tmpVertices[vi2];
                v3 = tmpVertices[vi1 + 1];
                v4 = tmpVertices[vi2 + 1];

                // if 1st stack and last stack, store only 1 triangle per sector
                // otherwise, store 2 triangles (quad) per sector
                if (i == 0) // a triangle for first stack -------------------------
                {
                    // add vertices of triangle
                    m_Vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                    m_Vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                    m_Vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

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
                    AddIndices(index, index + 1, index + 2);

                    index += 3; // get next index
                }
                else if (i == (m_StackCount - 1)) // a triangle for last stack --------
                {
                    // put a triangle
                    m_Vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                    m_Vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                    m_Vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));

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
                    AddIndices(index, index + 1, index + 2);

                    index += 3; // get next index
                }
                else // 2 triangles for others ----------------------------------
                {
                    // put quad vertices: v1-v2-v3-v4
                    m_Vertices.push_back(glm::vec3(v1.x, v1.y, v1.z));
                    m_Vertices.push_back(glm::vec3(v2.x, v2.y, v2.z));
                    m_Vertices.push_back(glm::vec3(v3.x, v3.y, v3.z));
                    m_Vertices.push_back(glm::vec3(v4.x, v4.y, v4.z));

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
                    AddIndices(index, index + 1, index + 2);
                    AddIndices(index + 2, index + 1, index + 3);

                    index += 4;
                }
            }
        }

        SetupTangentBitangents();
        SetupArrayBuffer();
        SetupMesh();
	}

}