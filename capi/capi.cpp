#include "capi.hpp"
#include "btBulletDynamicsCommon.h"

void* universeCreate() {
	universe* u = (universe*)malloc(sizeof(universe));
	u->collisionConfiguration = new btDefaultCollisionConfiguration();
	u->dispatcher = new btCollisionDispatcher(u->collisionConfiguration);
	u->broadphase = new btDbvtBroadphase();
	u->solver = new btSequentialImpulseConstraintSolver;
	u->dynamicsWorld = new btDiscreteDynamicsWorld(u->dispatcher, u->broadphase, u->solver, u->collisionConfiguration);

	// possibly could have an iteration callback function so all shapes in the
	// world get passed one at a time to a callback function(s)
	// only used for clean up for now...
	u->collisionShapes =  new btAlignedObjectArray<btCollisionShape*>();
	
	//float a;
	//btScalar b;
	//printf("sizeof float %i sizeof btScalar %i\n", sizeof(a),sizeof(b));

	return (void*)u;
}

void bodyRemove(void* u, void* b) {
	UNI(u)->dynamicsWorld->removeRigidBody(BODY(b));
}

void shapeDelete(void* u, void* s) {
	UNI(u)->collisionShapes->remove(SHAPE(s));
	delete SHAPE(s);
}

void bodyDelete(void* b) {
	if (BODY(b) && BODY(b)->getMotionState()) {
		delete BODY(b)->getMotionState();
	}
	delete BODY(b);
}

void* bodyGetShape(void* b) {
	return BODY(b)->getCollisionShape();
}

int compoundGetNumChildren(void* s) {
	return ((btCompoundShape*)s)->getNumChildShapes();
}

void compoundRemoveShape(void* s, int index) {
	((btCompoundShape*)s)->removeChildShapeByIndex(index);
}

void universeDestroy(void* u) {
	
	for (int i = UNI(u)->dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = UNI(u)->dynamicsWorld->getCollisionObjectArray().at(i);
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			while (body->getNumConstraintRefs())
			{
				btTypedConstraint* constraint = body->getConstraintRef(0);
				UNI(u)->dynamicsWorld->removeConstraint(constraint);
				delete constraint;
			}
			delete body->getMotionState();
		}
		UNI(u)->dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	for (int j = 0; j < UNI(u)->collisionShapes->size(); j++)
	{
		btCollisionShape* shape = (btCollisionShape*)(UNI(u)->collisionShapes->at(j));
		//collisionShapes[j] = 0;
		delete shape;
	}
	
	delete UNI(u)->dynamicsWorld;
	delete UNI(u)->solver;
	delete UNI(u)->broadphase;
	delete UNI(u)->dispatcher;
	delete UNI(u)->collisionConfiguration;
	UNI(u)->collisionShapes->clear();
	delete UNI(u)->collisionShapes;
	
	free(u);
}

void universeSetGravity(void* u, float x, float y, float z) {
	UNI(u)->dynamicsWorld->setGravity(btVector3(x,y,z));
}

void* shapeCreateBox(void* u, float ex, float ey, float ez) {
	btCollisionShape* shape = new btBoxShape(btVector3(btScalar(ex), btScalar(ey), btScalar(ez)));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void* shapeCreateCompound(void* u) {
	btCollisionShape* shape = new btCompoundShape();
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void compoundAddChild(void* compound, void* child, float x, float y, float z,
						float yaw, float pitch, float roll) {
	
	btTransform localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(x,y,z));
	
	btQuaternion quat;
	quat.setEuler(yaw, pitch, roll);
	localTrans.setRotation(quat);

	((btCompoundShape*)compound)->addChildShape(localTrans,SHAPE(child));
}

void* shapeCreateSphere(void* u, float re) {
	btCollisionShape* shape = new btSphereShape(re);
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;
}

void* shapeCreateCylinderZ(void* u,  float r, float l) {
	btCollisionShape* shape = new btCylinderShapeZ(btVector3(r,r,l));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;	
}

void* shapeCreateCylinderY(void* u,  float r, float l) {
	btCollisionShape* shape = new btCylinderShape(btVector3(r,l,r));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;	
}

void* shapeCreateCylinderX(void* u,  float r, float l) {
	btCollisionShape* shape = new btCylinderShapeX(btVector3(l,r,r));
	UNI(u)->collisionShapes->push_back(shape);
	return (void*)shape;	
}

void* hinge2Create(void* u, void* bodyA, void* bodyB, Vec anchor,
								Vec parentAxis, Vec childAxis, bool collide) {

		btVector3 anc(anchor.x, anchor.y, anchor.z);
		btVector3 pax(parentAxis.x, parentAxis.y, parentAxis.z);
		btVector3 cax(childAxis.x, childAxis.y, childAxis.z); 

		btHinge2Constraint* hinge2 = new btHinge2Constraint(
				*BODY(bodyA), *BODY(bodyB), 
				anc, pax, cax);
		UNI(u)->dynamicsWorld->addConstraint(hinge2, collide);

		//TODO add to global universe list of constraints for 
		// iteration / auto freeing on destroy universe
		
		return (void*)hinge2;
}

void hinge2setLowerLimit(void* h, float l) {
	((btHinge2Constraint*)h)->setLowerLimit(l);
}
void hinge2setUpperLimit(void* h, float l) {
	((btHinge2Constraint*)h)->setUpperLimit(l);
}

void hinge2enableMotor(void* h, int index, bool onOff) {
	((btHinge2Constraint*)h)->enableMotor(index, onOff);
}

void hinge2setMaxMotorForce(void* h, int index, float force) {
	((btHinge2Constraint*)h)->setMaxMotorForce(index, force);
}

void hinge2setTargetVelocity(void* h, int index, float vel) {
	((btHinge2Constraint*)h)->setTargetVelocity(index, vel);
}

void hinge2setDamping(void* h, int index, float damping, bool limitIfNeeded) {
	((btHinge2Constraint*)h)->setDamping(index, damping, limitIfNeeded);
}

void hinge2setStiffness(void* h, int index, float stiffness, bool limitIfNeeded) {
	((btHinge2Constraint*)h)->setStiffness(index, stiffness, limitIfNeeded);
}

float hinge2getAngle1(void* h) {
    return ((btHinge2Constraint*)h)->getAngle1();
}

float hinge2getAngle2(void* h) {
    return ((btHinge2Constraint*)h)->getAngle2();
}

void* hingeCreate(void* uni, void* bodyA, void* bodyB, 
					Vec pivA, Vec rotA, 
					Vec pivB, Vec rotB, bool refA, bool collide) {

	btTransform localA, localB;
	localA.setIdentity();
	localB.setIdentity();
	localA.getBasis().setEulerZYX(rotA.z, rotA.y, rotA.x);
	localA.setOrigin(btVector3(pivA.x, pivA.y, pivA.z));
	localB.getBasis().setEulerZYX(rotB.z, rotB.y, rotB.x);
	localB.setOrigin(btVector3(pivB.x, pivB.y, pivB.z));
	btHingeConstraint* hinge = new btHingeConstraint(
									*BODY(bodyA), *BODY(bodyB), 
									localA, localB, refA);
	UNI(uni)->dynamicsWorld->addConstraint(hinge, collide);
	// TODO research proper way to dispose of constraints later
	// possibly add to global constraint list for destroyUniverse			
	return (void*) hinge;

}


void hingeSetLimit(void* hinge, float low, float hi) {
	((btHingeConstraint*)hinge)->setLimit(low, hi);
}

void hingeEnableAngularMotor(void* hinge, bool enableMotor, 
						float targetVelocity, float maxMotorImpulse) {
	((btHingeConstraint*)hinge)->enableAngularMotor(enableMotor,
									targetVelocity, maxMotorImpulse);
}

// TODO can't get any sense from this...
//float hingeGetAngle(void* hinge) {
	//return ((btHingeConstraint*)hinge)->getHingeAngle();
	//return ((btHingeAccumulatedAngleConstraint*)hinge)->getAccumulatedHingeAngle();
	//btTransform transA = ((btHingeConstraint*)hinge)->getFrameOffsetA();
	//btTransform transB = ((btHingeConstraint*)hinge)->getFrameOffsetB();
	//return ((btHingeConstraint*)hinge)->getHingeAngle(transA, transB);
//}

void constraintSetParam(void* c, int num, float value, int axis) {
	JOINT(c)->setParam(num, value, axis);
}

bool constraintIsEnabled(void* c) {
	return JOINT(c)->isEnabled();
}
 
void constraintSetEnabled(void* c, bool en) {
	JOINT(c)->setEnabled(en);
}

void* bodyCreate(void* u, void* shape, float mass, float x, float y, float z) {
	// heavily "influenced" from bullet manual hello world console example
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(x, y, z));
	
	btScalar M(mass);
	bool isDynamic = (M != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		SHAPE(shape)->calculateLocalInertia(M, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(M, motionState, SHAPE(shape), localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	UNI(u)->dynamicsWorld->addRigidBody(body);
	return body;
}

void universeStep(void* u, float dt, int i) {
	UNI(u)->dynamicsWorld->stepSimulation(dt, i);
}

void bodyGetPositionAndOrientation(void* body, Vec* pos, Vec* r) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	pos->x = trans.getOrigin().getX();
	pos->y = trans.getOrigin().getY();
	pos->z = trans.getOrigin().getZ();	
	
	btQuaternion q = trans.getRotation();
	
	r->x = q.getX();
	r->y = q.getY();
	r->z = q.getZ();
	r->w = q.getW();
}

void bodyGetPosition(void* body, Vec* pos ) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	pos->x = trans.getOrigin().getX();
	pos->y = trans.getOrigin().getY();
	pos->z = trans.getOrigin().getZ();
}

void bodySetPosition(void* body, Vec pos ) {
	btTransform trans;
	trans.setOrigin(btVector3(pos.x,pos.y,pos.z));
	BODY(body)->setWorldTransform(trans);
}

void bodyGetOrientation(void* body, Vec* r) {
	btTransform trans;
	//if (BODY(body) && BODY(body)->getMotionState())	{
	//	BODY(body)->getMotionState()->getWorldTransform(trans);
	//} else {
		trans = BODY(body)->getWorldTransform();
	//}
	btQuaternion q = trans.getRotation();
	
	r->x = q.getX();
	r->y = q.getY();
	r->z = q.getZ();
	r->w = q.getW();
}

int bodyGetShapeType(void* body) {
	return BODY(body)->getCollisionShape()->getShapeType();
}

void bodyGetOpenGLMatrix(void* body, float* m) {
	btTransform trans;
	
	// TODO look into this...
	// commented only worked for dynamic bodies, getting world trans
	// direct from body works for both static and dynamic
	
//	if (BODY(body) && BODY(body)->getMotionState())	{
//		BODY(body)->getMotionState()->getWorldTransform(trans);
//	} else {
		trans = BODY(body)->getWorldTransform();
//	}
	trans.getOpenGLMatrix(m);
}

void bodyApplyImpulse(void* body, Vec* i, Vec* p) {
	BODY(body)->applyImpulse(btVector3(i->x,i->y,i->z), btVector3(p->x,p->y,p->z)); 	
}

void bodyApplyTorque(void* body, Vec* t) {
	BODY(body)->applyTorque(btVector3(t->x,t->y,t->z));
}

void bodySetRotationEular(void* body, float pitch, float yaw, float roll) {
	btQuaternion q = btQuaternion();
	q.setEuler(btScalar(yaw),btScalar(pitch),btScalar(roll));
	
	btTransform trans;

	trans = BODY(body)->getCenterOfMassTransform();
	trans.setRotation(q);
	
	BODY(body)->setCenterOfMassTransform(trans);
	
}

void bodySetRotation(void* body, Vec r) {
	bodySetRotationEular(body, r.x, r.y, r.z);
}

void bodySetRestitution(void* body, float r) {
	BODY(body)->setRestitution(r);
}

void bodyGetLinearVelocity(void* body, Vec* v) {
	const btVector3 bv = BODY(body)->getLinearVelocity();
	v->x = bv.getX();
	v->y = bv.getY();
	v->z = bv.getZ();
}

void bodySetLinearVelocity(void* body, Vec v) {
	BODY(body)->setLinearVelocity(btVector3(v.x,v.y,v.z));
}

void bodyGetAngularVelocity(void* body, Vec* v) {
	const btVector3 bv = BODY(body)->getAngularVelocity();
	v->x = bv.getX();
	v->y = bv.getY();
	v->z = bv.getZ();
}

void bodySetAngularVelocity(void* body, Vec v) {
	BODY(body)->setAngularVelocity(btVector3(v.x,v.y,v.z));
}

void bodySetFriction(void* s, float f) {
	BODY(s)->setFriction(f);
}

float bodyGetFriction(void* s) {
	return BODY(s)->getFriction();
}

void bodySetDeactivation(void* b, bool v) {
	if (v) {
		BODY(b)->forceActivationState(DEACTIVATION_ENABLE);
	} else {
		BODY(b)->forceActivationState(DEACTIVATION_DISABLE);
	}
}

void collisionCallback(void* u, void (*callback)(void*, void*, const Vec*, const Vec*, const Vec*) ) {
	int numManifolds = UNI(u)->dispatcher->getNumManifolds();
	
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = UNI(u)->dispatcher->getManifoldByIndexInternal(i);
        
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        int numContacts = contactManifold->getNumContacts();
        for (int j = 0; j < numContacts; j++) {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance() < 0.f) {
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
                
                const Vec pa={ptA.getX(),ptA.getY(),ptA.getZ()};
                const Vec pb={ptB.getX(),ptB.getY(),ptB.getZ()};
                const Vec n={normalOnB.getX(),normalOnB.getY(),normalOnB.getZ()};
                callback((void*)obA, (void*)obB, &pa, &pb, &n);
            }
        }
    }
}
