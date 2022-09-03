#pragma once

#include <Geometry/Mesh.h>
#include <vector>

namespace OP
{
	class Capsule : public Mesh
	{
	public:
		Capsule(float baseRadius = 1.0f, float topRadius = 1.0f, float height = 1.0f,
			int sectorCount = 36, int hemisphereStackCount = 18, int stackCount = 1, bool smooth = true);

		~Capsule() {}

		float GetBaseRadius()  const { return m_BaseRadius; }
		float GetTopRadius()   const { return m_TopRadius; }
		float GetHeight()      const { return m_Height; }
		float GetSectorCount() const { return m_SectorCount; }
		float GetStackCount()  const { return m_StackCount; }

		void Set(float baseRadius, float topRadius, float height,
			int sectorCount, int hemisphereStackCount, int stackCount, bool smooth = true);

		void SetBaseRadius(float baseRadius);
		void SetTopRadius(float topRadius);
		void SetHeight(float height);
		void SetSectorCount(int sectorCount);
		void SetStackCount(int stackCount);
		void SetSmooth(bool smooth);

		static Ref<Capsule> Create(float baseRadius = 1.0f, float topRadius = 1.0f, float height = 1.0f,
			int sectorCount = 36, int hemisphereStackCount = 18, int stackCount = 1, bool smooth = true);

	private:
		virtual void BuildVertices();
		void GenerateUnitCircleVertices();
		void BuildVerticesSmooth();
		void BuildVerticesFlat();
		std::vector<float> GetSideNormals();

		void SetHemispheres(uint32_t index);

		float m_BaseRadius;
		float m_TopRadius;
		float m_Height;
		int   m_SectorCount;
		int   m_StackCount;
		int   m_HemisphereStackCount;
		unsigned int m_BaseIndex;
		unsigned int m_TopIndex;
		bool m_Smooth;

		std::vector<float> m_UnitCircleVertices;
	};
}
