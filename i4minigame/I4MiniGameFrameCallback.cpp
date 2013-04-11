#include "stdafx.h"
#include "I4MiniGameFrameCallback.h"
#include "I4Framework.h"
#include "I4DeferredRenderer.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4Model.h"
#include "I4ModelMgr.h"
#include "I4PhysXMgr.h"
#include "I4ObjectMgr.h"
#include "i4ObjectNode.h"
#include "I4ObjectViewComponent.h"
#include "I4ObjectPointLightComponent.h"
#include "I4ObjectRigidBodyComponent.h"
#include "I4Messenger.h"
#include "I4ObjectFlyControllerComponent.h"
#include "I4ObjectStaticCameraComponent.h"
#include "I4ObjectCharacterMovementComponent.h"
#include "I4ObjectCharacterControllerComponent.h"
#include "I4ObjectTPSCameraComponent.h"
#include "I4FrameTimer.h"
#include "I4ModelElement.h"

using namespace i4core;


MiniGameFrameCallback::MiniGameFrameCallback()
: objectMgr(nullptr)
, renderer(nullptr)
, physXMgr(nullptr)
, player(nullptr)
{
}


MiniGameFrameCallback::~MiniGameFrameCallback()
{
}

bool MiniGameFrameCallback::onStart()
{
	Framework* framework = Framework::getFramework();
	renderer = new DeferredRenderer;
	if (renderer->initialize(framework->getWindowID(), framework->getWidth(), framework->getHeight()) == false)
		return false;

	renderer->getMainCamera().setPerspectiveFovLH(PI/4.0f, (float)framework->getWidth()/(float)framework->getHeight(), 0.1f, 100.0f);
	renderer->getMainCamera().setLookAtLH(Vector3(0, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));

	modelMgr = new ModelMgr;
	physXMgr = new PhysXMgr;
	if (physXMgr->init() == false)
		return false;

	objectMgr = new ObjectMgr;
	if (objectMgr->init(renderer, modelMgr, physXMgr) == false)
		return false;

	spectator =  objectMgr->createObject("spectator");
	spectator->setLookAtLH(Vector3(0.0f, 10.0f, -6.0f), Vector3(0.0f, 2.8f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

	ObjectFlyControllerComponent* spectatorController = spectator->addComponent<ObjectFlyControllerComponent>();
	spectatorController->activate(false);
	ObjectStaticCameraComponent* spectatorCamera = spectator->addComponent<ObjectStaticCameraComponent>();
	spectatorCamera->setMainCamera(false);

	player =  objectMgr->createObject("player");
	player->setPosition(Vector3(0.0f, 1.5f, -1.8f));
	player->setScale(Vector3(0.007f, 0.007f, 0.007f));

	ObjectViewComponent* playerView = player->addComponent<ObjectViewComponent>();
	playerView->attachModel("player", "testmodel/alice", true, true, true);
	playerView->attachAni("testmodel/alice.ani.xml", "idle");
	playerView->playAnimation("idle");

	Matrix4x4 m;
	m.makeRotationY(MathUtil::degreeToRadian(180));
	playerView->setOffset(m);
	
	ObjectCharacterMovementComponent* playerMovement = player->addComponent<ObjectCharacterMovementComponent>();
	playerMovement->attach(0.2f, 0.7f, cos(MathUtil::degreeToRadian(45)), 0.05f);
	playerMovement->setGravity(-9.8f);

	ObjectCharacterControllerComponent* playerController = player->addComponent<ObjectCharacterControllerComponent>();	
	playerController->activate(true);

	ObjectTPSCameraComponent* playerCamera = player->addComponent<ObjectTPSCameraComponent>();
	playerCamera->setMainCamera(true);

	Object* npc =  objectMgr->createObject("npc");
	npc->setPosition(Vector3(10.0f, 0.0f, -10.0f));
	npc->setScale(Vector3(0.01f, 0.01f, 0.01f));

	ObjectViewComponent* npcView = npc->addComponent<ObjectViewComponent>();
	npcView->attachModel("npc", "testmodel/marchhare", true, true, true);
	npcView->attachAni("testmodel/marchhare.ani.xml", "idle");
	npcView->playAnimation("idle");

	physXMgr->createPlane();

	Object* obj = nullptr;
	ObjectViewComponent* viewObj = nullptr; 
	ObjectRigidBodyComponent* rigidObj = nullptr;

	obj = objectMgr->createObject("floor");
	viewObj = obj->addComponent<ObjectViewComponent>();
	viewObj->attachModel("floor", "testmodel/floor", true, true, false);
	obj->setPosition(Vector3(0, 0, 0));

	Vector3 lightPointColor[] =
	{
		Vector3(1.0f, 0.125f, 0.93f),
		Vector3(1.0f, 0.0f, 0.0f),
		Vector3(0.0f, 0.8f, 0.8f),
		Vector3(1.0f, 1.0f, 0.0f),
		Vector3(1.0f, 0.5f, 0.25f),
		Vector3(0.0f, 0.125f, 0.93f),
		Vector3(1.0f, 0.0f, 0.5f),
		Vector3(1.0f, 0.8f, 0.8f),
		Vector3(0.5f, 1.0f, 0.3f),
		Vector3(0.7f, 0.5f, 0.25f),
		Vector3(0.0f, 1.0f, 0.0f),
		Vector3(0.7f, 0.9f, 0.75f),
		Vector3(0.5f, 0.7f, 0.7f),
	};
	
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int idx = i*10 + j;

			char decoName[256] = {0, };
			sprintf(decoName, "deco_%d", idx);
			obj = objectMgr->createObject(decoName);
			
			viewObj = obj->addComponent<ObjectViewComponent>();
			rigidObj = obj->addComponent<ObjectRigidBodyComponent>();

			Quaternion rot;
			rot.makeRotationAxis(VECTOR3_AXISY, MathUtil::degreeToRadian((float)(rand()%360)));
			obj->setRotation(rot);

			if (i%3 == 0)
			{
				obj->setPosition(Vector3(0.0f + i, j*0.15f, -5.0f));
				viewObj->attachModel(decoName, "testmodel/pallet", true, true, false);

				rigidObj->attachRepX("testmodel/pallet.RepX", 1.0f, false);
			}
			else if (i%3 == 1)
			{				
				obj->setPosition(Vector3(-20.0f + i*2, 0.5f + (float)(rand()%100)*0.05f, -20.0f + j*2));
				viewObj->attachModel(decoName, "testmodel/rollermine", true, true, false);

				rigidObj->attachRepX("testmodel/rollermine.RepX", 1.0f, true);
			}
			else
			{
				obj->setPosition(Vector3(-10.0f + i*1.5f, 0, j*1.5f));
				viewObj->attachModel(decoName, "testmodel/oildrum", true, true, false);				
				
				rigidObj->attachRepX("testmodel/oildrum.RepX", 1.0f, false);
			}

			char lightName[256] = {0, };
			sprintf(lightName, "light_%d", idx);
			objLight[idx] = objectMgr->createObject(lightName);

			ObjectPointLightComponent* lightPoint = objLight[idx]->addComponent<ObjectPointLightComponent>();
			lightPoint->setColor(lightPointColor[idx%13]);
			if ((i == 3 && j == 3) || (i == 6 && j == 6))
			{
				lightPoint->enableBlink(0.05f, 0.1f);
				lightPoint->setRadius(10);
			}
			else
			{
				lightPoint->setRadius(3.5f);
			}
		}
	}
	
	return true;
}

void MiniGameFrameCallback::onEnd()
{
	delete physXMgr;
	delete modelMgr;
	delete renderer;
	delete objectMgr;
}

bool MiniGameFrameCallback::onTick(float dt)
{
	PROFILE_THISFUNC;
	
	{
		PROFILE_BLOCK("onPreSimulate");

		MessageArgs preSimulateArgs;
		preSimulateArgs.push_back(dt);
		objectMgr->getMessenger().send(Hash("onPreSimulate"), preSimulateArgs);
	}
	
	{
		PROFILE_BLOCK("PhysX simulate");

		physXMgr->simulate(dt);
	}

	{
		PROFILE_BLOCK("onPostSimulate");

		MessageArgs postSimulateArgs;
		postSimulateArgs.push_back(dt);
		objectMgr->getMessenger().send(Hash("onPostSimulate"), postSimulateArgs);
	}
	

	return true;
}

bool MiniGameFrameCallback::onUpdate()
{
	PROFILE_THISFUNC;

	if (InputState::KeyPressed[VK_ESCAPE])
		return false;

	float dt = FrameTimer::getFrameTimer()->getFrameDelta();

	MessageArgs updateArgs;
	updateArgs.push_back(dt);
	objectMgr->getMessenger().send(Hash("onUpdateLogic"), updateArgs);
	
	static float degree = 0;
	degree += 30*dt;
	if (degree > 360.0f)
	{
		degree -= 360.0f;
	}
	
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int sign = 1;
			if (i%2 == 1)
			{
				sign = -1;
			}

			int idx = i*10 + j;
			
			if (i%2 == 0)
			{
				objLight[idx]->setPosition(Vector3(-20.0f + i*4.0f + sign*2.5f*cos(MathUtil::degreeToRadian(degree)),
					5.0f + sign*0.5f*cos(MathUtil::degreeToRadian(degree)),
					-30.0f + j*8.0f + sign*2.5f*sin(MathUtil::degreeToRadian(degree))));
			}
			else
			{
				objLight[idx]->setPosition(Vector3(-20.0f + i*4.0f + sign*2.5f*cos(MathUtil::degreeToRadian(degree)),
					1.0f + sign*0.5f*cos(MathUtil::degreeToRadian(degree)),
					-30.0f + j*8.0f + sign*2.5f*sin(MathUtil::degreeToRadian(degree))));
			}
		}
	}
	
	{
		PROFILE_BLOCK("onLateUpdate");
		MessageArgs readyToRenderArgs;
		objectMgr->getMessenger().send(Hash("onLateUpdate"), readyToRenderArgs);
	}


	return true;
}

bool MiniGameFrameCallback::onRender()
{
	PROFILE_THISFUNC;

	float dt = FrameTimer::getFrameTimer()->getFrameDelta();
	
	{
		PROFILE_BLOCK("onAnimate");
		MessageArgs aniArgs;
		aniArgs.push_back(dt);
		objectMgr->getMessenger().send(Hash("onAnimate"), aniArgs);
	}

	{
		PROFILE_BLOCK("onCommitToRenderer");
		MessageArgs renderArgs;
		objectMgr->getMessenger().send(Hash("onCommitToRenderer"), renderArgs);
	}
	
	if (renderer->isDebugMode())
	{
		PROFILE_BLOCK("physXMgr->commitDebugToRenderer(renderer)");
		physXMgr->commitDebugToRenderer(renderer);
	}

	{
		PROFILE_BLOCK("renderer->render()");
		renderer->render();
	}

	return true;
}

void MiniGameFrameCallback::onInput(const InputState& state)
{
	switch (state.type)
	{
	case INPUT_KEY_DOWN:
		{
			if (state.key == VK_F1)
			{
				bool isDebugMode = renderer->isDebugMode();

				renderer->setDebugMode(!isDebugMode);
				physXMgr->enableVisualization(!isDebugMode);
			}
			else if (state.key == VK_F2)
			{
				renderer->setWireMode(!renderer->isWireMode());
			}
			else if (state.key == VK_F3)
			{
				static bool isFree = false;

				isFree = !isFree;

				ObjectCharacterControllerComponent* playerController = player->findComponent<ObjectCharacterControllerComponent>();
				playerController->activate(!isFree);
				ObjectTPSCameraComponent* playerCam = player->findComponent<ObjectTPSCameraComponent>();
				playerCam->setMainCamera(!isFree);

				ObjectStaticCameraComponent* spectatorCamera = spectator->findComponent<ObjectStaticCameraComponent>();
				spectatorCamera->setMainCamera(isFree);

				ObjectFlyControllerComponent* spectatorController = spectator->findComponent<ObjectFlyControllerComponent>();
				spectatorController->activate(isFree);
			}
		}
		break;
	case INPUT_KEY_UP:
		break;
	case INPUT_LEFT_MOUSE_DOWN:
		{
			static int i = 0;
			char name[256] = {0, };
			sprintf(name, "physics_%d", i);
			Object* objPhysics = objectMgr->createObject(name);
			objPhysics->setPosition(Vector3(0, 5, 2));

			if (i%3 == 0)
			{
				ObjectViewComponent* view = objPhysics->addComponent<ObjectViewComponent>();
				view->attachModel(name, "testmodel/box", true, false, false);
				ObjectRigidBodyComponent* rigid = objPhysics->addComponent<ObjectRigidBodyComponent>();
				rigid->attachBox(Vector3(0.1f, 0.1f, 0.1f), 1, false);
			}
			else if (i%3 == 1)
			{
				ObjectRigidBodyComponent* rigid = objPhysics->addComponent<ObjectRigidBodyComponent>();
				rigid->attachSphere(0.1f, 1, false);
			}
			else if (i%3 == 2)
			{
				ObjectRigidBodyComponent* rigid = objPhysics->addComponent<ObjectRigidBodyComponent>();
				rigid->attachCapsule(0.3f, 1.0f, 1, false);
			}

			i++;
		}
		break;
	case INPUT_LEFT_MOUSE_UP:
		break;
	case INPUT_RIGHT_MOUSE_DOWN:
		break;
	case INPUT_RIGHT_MOUSE_UP:
		break;
	default:
		break;
	}
	
}