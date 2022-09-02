#pragma once

#include <Geometry/Mesh.h>
#include <vector>

namespace OP
{
	class Cylinder : public Mesh
	{
		Cylinder(float baseRadius = 1.0f, float topRadius = 1.0f, float height = 1.0f,
			     int sectorCount = 36, int stackCount = 1, bool smooth = true);

		~Cylinder() {}

		float GetBaseRadius()  const { return m_BaseRadius; }
		float GetTopRadius()   const { return m_TopRadius; }
		float GetHeight()      const { return m_Height; }
		float GetSectorCount() const { return m_SectorCount; }
		float GetStackCount()  const { return m_StackCount; }

		void Set(float baseRadius, float topRadius, float height,
			int sectorCount, int stackCount, bool smooth = true);

		void SetBaseRadius(float baseRadius);
		void SetTopRadius(float topRadius);
		void SetHeight(float height);
		void SetSectorCount(int sectorCount);
		void SetStackCount(int stackCount);
		void SetSmooth(bool smooth);

	private:
		virtual void BuildVertices();
		void GenerateUnitCircleVertices();
		void BuildVerticesSmooth();
		void BuildVerticesFlat();
		std::vector<float> GetSideNormals();
		float m_BaseRadius;
		float m_TopRadius;
		float m_Height;
		int   m_SectorCount;
		int   m_StackCount;
		unsigned int m_BaseIndex;
		unsigned int m_TopIndex;
		bool m_Smooth;

		std::vector<float> m_UnitCircleVertices;
	};
}
