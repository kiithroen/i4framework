#include "stdafx.h"
#include "I4MiniGameFrameCallback.h"
#include "I4Framework.h"
#include "I4DeferredRenderer.h"
#include "I4Camera.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4ProfileWriterLog.h"
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
#include "I4ObjectCharacterComponent.h"
#include "I4FrameTimer.h"

using namespace i4core;

I4MiniGameFrameCallback::I4MiniGameFrameCallback()
: objectMgr(nullptr)
, renderer(nullptr)
, physXMgr(nullptr)
, player(nullptr)
{
}

I4MiniGameFrameCallback::~I4MiniGameFrameCallback()
{
}

bool I4MiniGameFrameCallback::onStart()
{
	I4Framework* framework = I4Framework::getFramework();
	renderer = new I4DeferredRenderer;
	if (renderer->initialize(framework->getWindowID(), framework->getWidth(), framework->getHeight()) == false)
		return false;

	renderer->getMainCamera().setPerspectiveFov(I4PI/4.0f, (float)framework->getWidth()/(float)framework->getHeight(), 0.1f, 50.0f);

	modelMgr = new I4ModelMgr;
	physXMgr = new I4PhysXMgr;
	if (physXMgr->init() == false)
		return false;

	objectMgr = new I4ObjectMgr;
	if (objectMgr->init(renderer, modelMgr, physXMgr) == false)
		return false;
	
	framework->moveMouseCenter();

	spectator =  objectMgr->createNode("spectator");
	spectator->setLocalLookAt(I4Vector3(0.0f, 10.0f, -6.0f), I4Vector3(0.0f, 2.8f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));

	I4ObjectFlyControllerComponent* spectatorController = spectator->addComponent<I4ObjectFlyControllerComponent>();
	spectatorController->activate(false);
	I4ObjectStaticCameraComponent* spectatorCamera = spectator->addComponent<I4ObjectStaticCameraComponent>();
	spectatorCamera->setMainCamera(false);

	player =  objectMgr->createNode("player");
	player->setLocalLookAt(I4Vector3(0.0f, 3.0f, -1.8f), I4Vector3(0.0f, 3.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));
	player->setLocalScale(I4Vector3(0.03f, 0.03f, 0.03f));

	I4ObjectViewComponent* view = player->addComponent<I4ObjectViewComponent>();
	view->attachModel("player", "testmodel/elin", true, true, false);

	I4ObjectCharacterComponent* playerCharacter = player->addComponent<I4ObjectCharacterComponent>();
	playerCharacter->attach(0.2f, 0.7f, cos(I4MathUtil::degreeToRadian(70)), 0.1f);
	
	I4ObjectStaticCameraComponent* playerCamera = player->addComponent<I4ObjectStaticCameraComponent>();
	playerCamera->setMainCamera(true);

	I4ObjectNode* nodeFloor = objectMgr->createNode("floor");
	//nodeFloor->setLocalScale(I4Vector3(0.1f, 0.1f, 0.1f));
	I4ObjectViewComponent* viewFloor = nodeFloor->addComponent<I4ObjectViewComponent>();
	viewFloor->attachModel("floor", "testmodel/floor", true, true, false);

	physXMgr->createPlane();
	
	I4Vector3 lightPointColor[] =
	{
		I4Vector3(1.0f, 0.125f, 0.93f),
		I4Vector3(1.0f, 0.0f, 0.0f),
		I4Vector3(0.0f, 0.8f, 0.8f),
		I4Vector3(1.0f, 1.0f, 0.0f),
		I4Vector3(1.0f, 0.5f, 0.25f),
		I4Vector3(0.0f, 0.125f, 0.93f),
		I4Vector3(1.0f, 0.0f, 0.5f),
		I4Vector3(1.0f, 0.8f, 0.8f),
		I4Vector3(0.5f, 1.0f, 0.3f),
		I4Vector3(0.7f, 0.5f, 0.25f),
		I4Vector3(0.0f, 1.0f, 0.0f),
		I4Vector3(0.7f, 0.9f, 0.75f),
		I4Vector3(0.5f, 0.7f, 0.7f),
	};

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int idx = i*10 + j;

			char charName[256] = {0, };
			sprintf(charName, "char_%d", idx);
			I4ObjectNode* nodeChar = objectMgr->createNode(charName);
			nodeChar->setLocalPosition(I4Vector3(-20.0f + i*4.0f, 0.0f, -20.0f + j*4.0f));

			I4ObjectViewComponent* view = nodeChar->addComponent<I4ObjectViewComponent>();
			I4ObjectRigidBodyComponent* rigid = nodeChar->addComponent<I4ObjectRigidBodyComponent>();

			if (i%3 == 0)
			{
				nodeChar->setLocalScale(I4Vector3(0.006f, 0.006f, 0.006f));

				view->attachModel(charName, "testmodel/cyberdemon", true, true, true);
				view->attachAni("testmodel/cyberdemon.ani.xml", "idle");
				view->playAnimation("idle");	

				I4Matrix4x4 offset;
				offset.makeTranslation(0, -0.85f, 0);
				rigid->setOffset(offset);
				rigid->attachCapsule(0.4f, 0.9f, 1, false);
			}
			else if (i%3 == 1)
			{
				nodeChar->setLocalScale(I4Vector3(0.01f, 0.01f, 0.01f));

				view->attachModel(charName, "testmodel/guard", true, true, true);
				view->attachAni("testmodel/guard_idle.ani.xml", "idle");
				view->playAnimation("idle");

				I4Matrix4x4 offset;
				offset.makeTranslation(0, -0.65f, 0);
				rigid->setOffset(offset);
				rigid->attachCapsule(0.3f, 0.7f, 1, false);
			}
			else
			{
				nodeChar->setLocalScale(I4Vector3(0.03f, 0.03f, 0.03f));

				view->attachModel(charName, "testmodel/elin", true, true, false);
				I4Matrix4x4 offset;
				offset.makeTranslation(0, -0.55f, 0.1f);
				rigid->setOffset(offset);
				rigid->attachCapsule(0.2f, 0.7f, 1, false);
			}

			char lightName[256] = {0, };
			sprintf(lightName, "light_%d", idx);
			nodeLight[idx] = objectMgr->createNode(lightName);
			nodeLight[idx]->setLocalPosition(I4Vector3(-20.0f + i*4.0f, 0.5f, -20.0f + j*4.0f));

			I4ObjectPointLightComponent* lightPoint = nodeLight[idx]->addComponent<I4ObjectPointLightComponent>();
			lightPoint->setColor(lightPointColor[idx%13]);
			if ((i == 3 && j == 3) || (i == 6 && j == 6))
			{
				lightPoint->enableBlink(0.05f, 0.1f);
				lightPoint->setRadius(5);
			}
			else
			{
				lightPoint->setRadius(2.5f);
			}
		}
	}

	return true;
}

void I4MiniGameFrameCallback::onEnd()
{
	delete physXMgr;
	delete modelMgr;
	delete renderer;
	delete objectMgr;
}

bool I4MiniGameFrameCallback::onSimulate(float dt)
{
	I4PROFILE_THISFUNC;
	
	I4MessageArgs preSimulateArgs;
	preSimulateArgs.push_back(dt);
	objectMgr->getMessenger().send(I4Hash("onPreSimulate"), preSimulateArgs);

	physXMgr->simulate(dt);

	I4MessageArgs postSimulateArgs;
	postSimulateArgs.push_back(dt);
	objectMgr->getMessenger().send(I4Hash("onPostSimulate"), postSimulateArgs);

	return true;
}

bool I4MiniGameFrameCallback::onUpdate()
{
	if (I4InputState::KeyPressed[VK_ESCAPE])
		return false;

	float dt = I4FrameTimer::getFrameTimer()->getDeltaSec();

	I4MessageArgs updateArgs;
	updateArgs.push_back(dt);
	objectMgr->getMessenger().send(I4Hash("onUpdate"), updateArgs);

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

			nodeLight[idx]->setLocalPosition(I4Vector3(-18.0f + i*4.0f + sign*2.5f*cos(I4MathUtil::degreeToRadian(degree)),
				1.0f + sign*0.5f*cos(I4MathUtil::degreeToRadian(degree)),
				-18.0f + j*4.0f + sign*2.5f*sin(I4MathUtil::degreeToRadian(degree))));
		}
	}


	I4MessageArgs postUpdateArgs;
	postUpdateArgs.push_back(dt);
	objectMgr->getMessenger().send(I4Hash("onPostUpdate"), postUpdateArgs);

	static float elapsed = 0;

	elapsed += dt;

	if (elapsed >= 1.0f)
	{
		I4ProfileWriterLog	writer;
		writer.write(I4ProfileManager::getRootNode());

		elapsed = 0;
		I4ProfileManager::clear();
	}

	return true;
}

bool I4MiniGameFrameCallback::onRender()
{	
	float dt = I4FrameTimer::getFrameTimer()->getDeltaSec();

	I4MessageArgs aniArgs;
	aniArgs.push_back(dt);
	objectMgr->getMessenger().send(I4Hash("onAnimate"), aniArgs);

	I4MessageArgs readyToRenderArgs;
	objectMgr->getMessenger().send(I4Hash("onReadyToRender"), readyToRenderArgs);

	I4MessageArgs renderArgs;
	objectMgr->getMessenger().send(I4Hash("onRender"), renderArgs);


	if (renderer->isDebugMode())
	{
		physXMgr->commitDebugToRenderer(renderer);
	}

	renderer->render();

	return true;
}

void I4MiniGameFrameCallback::onInput(const I4InputState& state)
{
	switch (state.type)
	{
	case I4INPUT_KEY_DOWN:
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

				I4ObjectCharacterComponent* playerController = player->findComponent<I4ObjectCharacterComponent>();

				I4ObjectStaticCameraComponent* playerCam = player->findComponent<I4ObjectStaticCameraComponent>();
				playerCam->setMainCamera(!isFree);

				I4ObjectStaticCameraComponent* spectatorCamera = spectator->findComponent<I4ObjectStaticCameraComponent>();
				spectatorCamera->setMainCamera(isFree);

				I4ObjectFlyControllerComponent* spectatorController = spectator->findComponent<I4ObjectFlyControllerComponent>();
				spectatorController->activate(isFree);
			}
		}
		break;
	case I4INPUT_KEY_UP:
		break;
	case I4INPUT_LEFT_MOUSE_DOWN:
		{
			static int i = 0;
			char name[256] = {0, };
			sprintf(name, "physics_%d", i);
			I4ObjectNode* nodePhysics = objectMgr->createNode(name);
			nodePhysics->setLocalPosition(I4Vector3(0, 5, 2));

			if (i%3 == 0)
			{
				I4ObjectViewComponent* view = nodePhysics->addComponent<I4ObjectViewComponent>();
				view->attachModel(name, "testmodel/box", true, false, false);
				I4ObjectRigidBodyComponent* rigid = nodePhysics->addComponent<I4ObjectRigidBodyComponent>();
				rigid->attachBox(I4Vector3(0.1f, 0.1f, 0.1f), 1, false);
			}
			else if (i%3 == 1)
			{
				I4ObjectRigidBodyComponent* rigid = nodePhysics->addComponent<I4ObjectRigidBodyComponent>();
				rigid->attachSphere(0.1f, 1, false);
			}
			else if (i%3 == 2)
			{
				I4ObjectRigidBodyComponent* rigid = nodePhysics->addComponent<I4ObjectRigidBodyComponent>();
				rigid->attachCapsule(0.3f, 1.0f, 1, false);
			}

			i++;
		}
		break;
	case I4INPUT_LEFT_MOUSE_UP:
		break;
	case I4INPUT_RIGHT_MOUSE_DOWN:
		break;
	case I4INPUT_RIGHT_MOUSE_UP:
		break;
	default:
		break;
	}
	
}

void I4MiniGameFrameCallback::updateCamera(float dt)
{

}