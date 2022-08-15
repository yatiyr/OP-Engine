#include <Precomp.h>
#include <PhysicsManager/PhysicsManager.h>


namespace OP
{

	struct PhysicsManagerData
	{
		btAlignedObjectArray<btCollisionShape*>  CollisionShapes;
		btBroadphaseInterface*                   Broadphase;
		btCollisionDispatcher*                   Dispatcher;
		btConstraintSolver*                      Solver;
		btDefaultCollisionConfiguration*         CollisionConfiguration;
		btDiscreteDynamicsWorld*                 DynamicsWorld;

	} s_PMD; // s_PhysicsManagerData

	void PhysicsManager::Init()
	{
		OP_ENGINE_WARN("Initializing Physics Manager");
			s_PMD.CollisionConfiguration = new btDefaultCollisionConfiguration();

			// we use default collision dispatcher
			s_PMD.Dispatcher = new btCollisionDispatcher(s_PMD.CollisionConfiguration);
			s_PMD.Broadphase = new btDbvtBroadphase();

			// default constraint solver
			btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
			s_PMD.Solver = sol;

			s_PMD.DynamicsWorld = new btDiscreteDynamicsWorld(s_PMD.Dispatcher, s_PMD.Broadphase, s_PMD.Solver, s_PMD.CollisionConfiguration);

			s_PMD.DynamicsWorld->setGravity(btVector3(0.0, 0.0, -10.0f));


		OP_ENGINE_WARN("Physics Manager has been initialized");
	}

	void PhysicsManager::StepWorld()
	{
	}

}