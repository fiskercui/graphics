/*
 * http://www.cnblogs.com/liangliangh/
 --------------------------------------------------------------------------------*/

#include"bt_inc.h"
#include"gl_inc.h"
#include"omp.h"

btDiscreteDynamicsWorld* m_DynamicsWorld;
btBroadphaseInterface* m_Broadphase;
btCollisionDispatcher* m_Dispatcher;
btSequentialImpulseConstraintSolver* m_ConstraintSolver;
btDefaultCollisionConfiguration* m_CollisionConfiguration;
btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;

void bt_rundraw(bool run)
{
	static double t_Last = omp_get_wtime();
	if(run){
		double t2 = omp_get_wtime();
		m_DynamicsWorld->stepSimulation(float(t2-t_Last),10);
		t_Last = t2;
	}else{
		t_Last = omp_get_wtime();
	}

	btCollisionObjectArray& rigidArray = m_DynamicsWorld->getCollisionObjectArray();
	for(int i=0; i<rigidArray.size(); ++i){
		btRigidBody* body = btRigidBody::upcast(rigidArray[i]);
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		float m[16];
		trans.getOpenGLMatrix(m);
		GLfloat color[]={.5f, .6f, .7f, 1.0f};
		if(i==0){
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(m);
			glTranslatef(0,-1,0);
			glScalef(100.0f,1.0f,100.0f);
			glutSolidCube(1.f);
			glPopMatrix();
		}else{
			if(i%2){
				color[0]=0.0f;color[1]=0.9f;color[2]=0.0f;color[3]=1.0f;
			}else{
				color[0]=0.9f;color[1]=0.0f;color[2]=0.0f;color[3]=1.0f;
			}
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMultMatrixf(m);
			glScalef(3.0f,2.0f,4.0f);
			glutSolidCube(1.f);
			glPopMatrix();
		}
	}
}

void bt_start()
{
	///-----initialization_start-----
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();
	m_Dispatcher = new	btCollisionDispatcher(m_CollisionConfiguration);
	m_Broadphase = new btDbvtBroadphase();
	m_ConstraintSolver = new btSequentialImpulseConstraintSolver;
	m_DynamicsWorld = new btDiscreteDynamicsWorld(
		m_Dispatcher,m_Broadphase,m_ConstraintSolver,m_CollisionConfiguration);
	m_DynamicsWorld->setGravity(btVector3(0,-10,0));
	///-----initialization_end-----

	{ // floor
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.f),btScalar(0.f),btScalar(50.f)));
		m_CollisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,0,0));
		btScalar mass(0.f);

		btVector3 localInertia(0,0,0);
		if( mass != 0.f )
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		m_DynamicsWorld->addRigidBody(body);
	}

	for(int i=0; i<20; ++i){
		btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(1.5f),btScalar(1.f),btScalar(2.f)));
		m_CollisionShapes.push_back(boxShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,i*2.0f+1.0f,i*0.5f));
		btScalar mass(6.f);

		btVector3 localInertia(0,0,0);
		if( mass != 0.f )
			boxShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,boxShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		m_DynamicsWorld->addRigidBody(body);
	}
	
}

void bt_end()
{
	//remove the rigidbodies from the dynamics world and delete them
	for (int i=m_DynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();
		m_DynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
	//delete collision shapes
	for (int i=0;i<m_CollisionShapes.size();i++)
	{
		btCollisionShape* shape = m_CollisionShapes[i];
		m_CollisionShapes[i] = 0;
		delete shape;
	}
	//delete dynamicsworld and ...
	delete m_DynamicsWorld;
	delete m_ConstraintSolver;
	delete m_Broadphase;
	delete m_Dispatcher;
	delete m_CollisionConfiguration;
	m_CollisionShapes.clear();
}
