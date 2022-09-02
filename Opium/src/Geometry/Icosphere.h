#pragma once

#include <Geometry/Mesh.h>

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtc/matrix_transform.hpp>


namespace OP
{
	class Icosphere : public Mesh
	{
	public:
		Icosphere(float radius = 1.0f, int subdivision = 1, bool smooth = true);
		~Icosphere() {}

		int GetSubdivision() const { return m_Subdivision; }

		void Set(float radius, int subdivision);
		void SetRadius(float radius);
		void SetSubdivision(int subdivision);
		void UpdateRadius();
		void PrintSelf() const;

		static Ref<Icosphere> Create(float radius = 1.0f, int subdivision = 1, bool smooth = true);

	private:

		virtual void BuildVertices();

		void BuildVerticesSmooth();
		void BuildVerticesFlat();

		void SubdivideVerticesSmooth();
		void SubdivideVerticesFlat();

		std::vector<glm::vec3> ComputeIcosahedronVertices();

		void ComputeVertexNormal(const glm::vec3& vertex, glm::vec3& normal);
		void ComputeHalfVertex(const glm::vec3& v1, const glm::vec3& v2, float length, glm::vec3& newVertex);
		void ComputeHalfTexCoord(const glm::vec2& t1, const glm::vec2& t2, glm::vec2& newTexCoord);

		float ComputeScaleForLength(glm::vec3 vertex, float length);
		bool IsSharedTexCoord(const glm::vec2& t);
		bool IsOnLineSegment(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

		unsigned int AddSubVertexAttributes(const glm::vec3& v, const glm::vec3& n, const glm::vec2& t);

		int m_Subdivision;
		int m_Radius;
		std::map<std::pair<float, float>, unsigned int> m_SharedIndices;

	};
}