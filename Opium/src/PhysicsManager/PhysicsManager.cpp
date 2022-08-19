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

		btRigidBody* sphere;
		btRigidBody* ground;
	} s_PMD; // s_PhysicsManagerData


	static bool CollisionCallbackFunction(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1,
														const btCollisionObjectWrapper* obj2, int id2, int index2)
	{
		const btCollisionObject* cObj1 = obj1->getCollisionObject();
		const btCollisionObject* cObj2 = obj2->getCollisionObject();

		// cp.m_index0

		PhysicsObject* o1 = (PhysicsObject*)cObj1->getUserPointer();
		PhysicsObject* o2 = (PhysicsObject*)cObj1->getUserPointer();


		OP_ENGINE_WARN("Obj1 id: {0}, Obj2 id: {1}", o1->id, o2->id);
		return false;
	}

	static void CollisionStartedCallbackFunc(btPersistentManifold* const& manifold)
	{
		OP_ENGINE_WARN("CollisionStarted");

		const btCollisionObject* b1 = manifold->getBody0();
		const btCollisionObject* b2 = manifold->getBody1();

		PhysicsObject* o1 = (PhysicsObject*)b1->getUserPointer();
		PhysicsObject* o2 = (PhysicsObject*)b2->getUserPointer();


		OP_ENGINE_WARN("Obj1 id: {0}, Obj2 id: {1}", o1->id, o2->id);

	}


	static void CollisionEndedCallbackFunc(btPersistentManifold* const& manifold)
	{
		OP_ENGINE_WARN("CollisionEnded");

		const btCollisionObject* b1 = manifold->getBody0();
		const btCollisionObject* b2 = manifold->getBody1();

		PhysicsObject* o1 = (PhysicsObject*)b1->getUserPointer();
		PhysicsObject* o2 = (PhysicsObject*)b2->getUserPointer();


		OP_ENGINE_WARN("Obj1 id: {0}, Obj2 id: {1}", o1->id, o2->id);
	}

	void PhysicsManager::Init()
	{
		OP_ENGINE_WARN("Initializing Physics Manager");
			// register callback function
			gContactAddedCallback = CollisionCallbackFunction;
			gContactStartedCallback = CollisionStartedCallbackFunc;
			gContactEndedCallback = CollisionEndedCallbackFunc;

			// -------------------- TEST PART ---------------------- //



			// ----------------------------------------------------- //
		OP_ENGINE_WARN("Physics Manager has been initialized");
	}

	void PhysicsManager::DeleteRigidBody(void* runtimeBody, int collisionShapeIndex)
	{
		btRigidBody* rB = (btRigidBody*)runtimeBody;

		btCollisionShape* colShape = s_PMD.CollisionShapes[collisionShapeIndex];

		delete rB->getMotionState();
		s_PMD.DynamicsWorld->removeCollisionObject(rB);

		delete colShape;
		colShape = nullptr;
	}

	AddRigidBodyResponse PhysicsManager::AddRigidBody(const RigidBodySpec& spec, void* motionState, void* body, void* collisionShape, void* EntityPointer)
	{
		int       shape        = spec.Shape;
		btVector3 scale        = spec.scale;
		float     radius       = spec.Radius;

		float mass             = spec.Mass;
		float friction         = spec.Friction;
		float rollingFriction  = spec.RollingFriction;
		float spinningFriction = spec.SpinningFriction;
		float restitution      = spec.Restitution;

		btVector3 origin         = spec.Origin;
		btQuaternion orientation = spec.Orientation;

		bool contactResponse = spec.ContactResponse;

		btCollisionShape* colShape = nullptr;
		if      (shape == 0) {colShape = new btBoxShape(scale/2);}
		else if (shape == 1) {colShape = new btSphereShape(radius);}
		s_PMD.CollisionShapes.push_back(colShape);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(origin);
		transform.setRotation(orientation);

		bool isDynamic = (mass != 0.0);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* mS = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mS, colShape, localInertia);
		btRigidBody* rB = new btRigidBody(rbInfo);

		motionState    = (void*)mS;
		body           = (void*)rB;
		collisionShape = (void*)colShape;

		rB->setRestitution(restitution);
		rB->setFriction(friction);
		rB->setRollingFriction(rollingFriction);
		rB->setSpinningFriction(spinningFriction);
		rB->setAnisotropicFriction(rB->getAnisotropicFriction(), btCollisionObject::CF_ANISOTROPIC_FRICTION);
		rB->setCollisionFlags(rB->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK
													 );

		rB->setUserPointer(EntityPointer);

		s_PMD.DynamicsWorld->addRigidBody(rB);

		return { mS, rB, colShape,s_PMD.CollisionShapes.size() - 1 };
	}

	void PhysicsManager::StartWorld()
	{
		// INITIALIZE DYNAMICS WORLD
			s_PMD.CollisionConfiguration = new btDefaultCollisionConfiguration();

			// we use default collision dispatcher
			s_PMD.Dispatcher = new btCollisionDispatcher(s_PMD.CollisionConfiguration);
			s_PMD.Broadphase = new btDbvtBroadphase();

			// default constraint solver
			btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
			s_PMD.Solver = sol;

			s_PMD.DynamicsWorld = new btDiscreteDynamicsWorld(s_PMD.Dispatcher, s_PMD.Broadphase, s_PMD.Solver, s_PMD.CollisionConfiguration);

			s_PMD.DynamicsWorld->setGravity(btVector3(0.0, -10.0, 0.0));


	}

	void PhysicsManager::ClearWorld()
	{
		// Clear the allocated objects
		OP_ENGINE_WARN("Cleaning The Physical World");
		for (int i = s_PMD.DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = s_PMD.DynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			s_PMD.DynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		// delete collision shapes
		for (int i = 0; i < s_PMD.CollisionShapes.size(); i++)
		{
			btCollisionShape* shape = s_PMD.CollisionShapes[i];
			if(shape)
				delete shape;
		}
		s_PMD.CollisionShapes.clear();

		delete s_PMD.DynamicsWorld;
		s_PMD.DynamicsWorld = nullptr;

		delete s_PMD.Solver;
		s_PMD.Solver = nullptr;

		delete s_PMD.Broadphase;
		s_PMD.Solver = nullptr;

		delete s_PMD.Dispatcher;
		s_PMD.Dispatcher = nullptr;

		delete s_PMD.CollisionConfiguration;
		s_PMD.CollisionConfiguration = nullptr;
	}

	void PhysicsManager::StepWorld(float ts)
	{
		s_PMD.DynamicsWorld->stepSimulation(ts);
	}

}