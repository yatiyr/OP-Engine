#pragma once

#include <btBulletDynamicsCommon.h>

namespace OP
{
	struct AddRigidBodyResponse
	{
		void* motionState;
		void* body;
		void* collisionShape;
		int collisionShapeIndex;
	};
	struct RigidBodySpec
	{
		float Mass;
		float Friction;
		float RollingFriction;
		float SpinningFriction;
		float Restitution;

		// Shape = 0 -> Box
		// Shape = 1 -> Sphere
		int Shape = 0;

		btVector3 scale;
		float Radius;

		bool ContactResponse;

		btQuaternion Orientation;
		btVector3 Origin;
	};

	struct PhysicsObject
	{
		uint32_t id;
		bool hit;
		btRigidBody* body;
		void* entity;

		PhysicsObject(btRigidBody* b, int id, void* ent) : body(b), id(id), entity(ent), hit(false) {}
	};


	class PhysicsManager
	{
	public:

		static void Init();

		static AddRigidBodyResponse AddRigidBody(const RigidBodySpec& spec, void* motionState, void* body, void* collisionShape, void* EntityPointer);
		static void DeleteRigidBody(void* runtimeBody, int collisionShapeIndex);
		static void StartWorld();
		static void ClearWorld();
		static void StepWorld(float ts);
	};
}