#pragma once

#include <Geometry/Mesh.h>


namespace OP
{
	class RadialSphere : public Mesh
	{
	public:
		RadialSphere(float radius = 1.0f, int sectorCount = 64, int stackCount = 64, bool smooth = true);
		~RadialSphere();


		float GetSectorCount() const { return m_SectorCount; }
		float GetStackCount()  const { return m_StackCount; }

		void Set(float radius, int sectorCount, int stackCount);
		void SetRadius(float radius);
		void SetSectorCount(int sectorCount);
		void SetStackCount(int stackCount);

		void PrintSelf() const;


		static Ref<RadialSphere> Create(float radius = 1.0f, int sectorCount = 64, int stackCount = 64, bool smooth = true);

	private:
		virtual void BuildVertices();

		void BuildVerticesSmooth();
		void BuildVerticesFlat();

		

		int m_Radius;
		int m_SectorCount;
		int m_StackCount;

	};
}