#pragma once

#include <btBulletDynamicsCommon.h>

namespace OP
{
	class PhysicsManager
	{
	public:

		static void Init();
		static void StepWorld();
	};
}