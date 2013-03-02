#include "stdafx.h"
#include "I4MiniGameFrameCallback.h"
#include "I4MiniGameState_Game.h"
#include "I4Framework.h"
#include "I4DeferredRenderer.h"
#include "I4Camera.h"
#include "I4FrameStateMgr.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4ProfileWriterLog.h"
#include "I4Model.h"
#include "I4ModelMgr.h"
#include "I4ObjectMgr.h"
#include "i4ObjectNode.h"
#include "I4ObjectViewComponent.h"
#include "I4Messenger.h"
using namespace i4core;

I4MiniGameFrameCallback::I4MiniGameFrameCallback()
: frameStateMgr(nullptr)
, objectMgr(nullptr)
, renderer(nullptr)
, camera(nullptr)
, isRButtonDown(false)
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
	
	camera = new I4Camera;
	camera->setPerspectiveFov(I4PI/4.0f, (float)framework->getWidth()/(float)framework->getHeight(), 0.1f, 50.0f);
	camera->setLookAt(I4Vector3(0.0f, 3.0f, -0.8f), I4Vector3(0.0f, 2.8f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));  
	
	float camYawRad;
	float camPitchRad;
	float camRollRad;
	camera->getRotation().extractYawPitchRoll(camYawRad, camPitchRad, camRollRad);

	camYaw = I4MathUtil::radianToDegree(camYawRad);
	camPitch = I4MathUtil::radianToDegree(camPitchRad);
	camRoll = I4MathUtil::radianToDegree(camRollRad);

	modelMgr = new I4ModelMgr;
	objectMgr = new I4ObjectMgr;
	if (objectMgr->init(renderer, modelMgr) == false)
		return false;

	frameStateMgr = new I4FrameStateMgr;
	frameStateMgr->addFrameState("minigame", new I4MiniGameState_Game);
	frameStateMgr->changeFrameState("minigame");

	framework->moveMouseCenter();
	framework->getMousePos(prevMouseX, prevMouseY);

	I4ObjectNode* nodeFloor = objectMgr->createNode("floor");
	nodeFloor->setLocalScale(I4Vector3(0.1f, 0.1f, 0.1f));
	I4ObjectViewComponent* viewFloor = nodeFloor->addComponent<I4ObjectViewComponent>();
	if (viewFloor)
	{
		viewFloor->attachModel("floor", "testmodel/floor", true, true, false);
	}

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int idx = i*10 + j;

			char name[256] = {0, };
			sprintf(name, "object_%d", idx);
			I4ObjectNode* node = objectMgr->createNode(name);

			I4ObjectViewComponent* view = node->addComponent<I4ObjectViewComponent>();
			if (view)
			{
				if (i%3 == 0)
				{
					node->setLocalScale(I4Vector3(0.006f, 0.006f, 0.006f));

					view->attachModel(name, "testmodel/cyberdemon", true, true, true);
					view->attachAni("testmodel/cyberdemon.ani.xml", "idle");
					view->playAnimation("idle");
				}
				else if (i%3 == 1)
				{
					node->setLocalScale(I4Vector3(0.01f, 0.01f, 0.01f));

					view->attachModel(name, "testmodel/guard", true, true, true);
					view->attachAni("testmodel/guard_idle.ani.xml", "idle");
					view->playAnimation("idle");
				}
				else
				{
					node->setLocalScale(I4Vector3(0.03f, 0.03f, 0.03f));

					view->attachModel(name, "testmodel/elin", true, true, false);
				}
			}

			node->setLocalPosition(I4Vector3(-20.0f + i*4.0f, 0.0f, -20.0f + j*4.0f));
		}
	}
	

	return true;
}

void I4MiniGameFrameCallback::onEnd()
{
	delete modelMgr;
	delete renderer;
	delete camera;
	delete objectMgr;
	delete frameStateMgr;
}

bool I4MiniGameFrameCallback::onUpdate(float deltaSec)
{
	I4Framework* framework = I4Framework::getFramework();
	
	if (framework->isKeyPressed(VK_ESCAPE))
		return false;

	if (frameStateMgr == nullptr)
		return true; 

	if (frameStateMgr->onUpdate(deltaSec) == false)
		return false;


	updateCamera(deltaSec);

	static float elapsed = 0;

	elapsed += deltaSec;

	if (elapsed >= 1.0f)
	{
		I4ProfileWriterLog	writer;
		writer.write(I4ProfileManager::getRootNode());

		elapsed = 0;
		I4ProfileManager::reset();
	}

	return true;
}

bool I4MiniGameFrameCallback::onRender(float deltaSec)
{
	commitToRenderer(deltaSec);

	renderer->preRender(camera);
	renderer->render(camera);
	renderer->postRender(camera);

	return true;
}

void I4MiniGameFrameCallback::onKeyDown(unsigned int key)
{
	if (key == VK_F2)
	{
		renderer->setWireMode(!renderer->isWireMode());
	}

	if (frameStateMgr)
	{
		frameStateMgr->onKeyDown(key);
	}
}

void I4MiniGameFrameCallback::onKeyUp(unsigned int key)
{
	if (frameStateMgr)
	{
		frameStateMgr->onKeyUp(key);
	}
}

void I4MiniGameFrameCallback::onMouseMove(unsigned int x, unsigned int y)
{
	if (frameStateMgr)
	{
		frameStateMgr->onMouseMove(x, y);
	}
}

void I4MiniGameFrameCallback::onLButtonDown(unsigned int x, unsigned int y)
{
	if (frameStateMgr)
	{
		frameStateMgr->onLButtonDown(x, y);
	}
}

void I4MiniGameFrameCallback::onLButtonUp(unsigned int x, unsigned int y)
{
	if (frameStateMgr)
	{
		frameStateMgr->onLButtonUp(x, y);
	}
}

void I4MiniGameFrameCallback::onRButtonDown(unsigned int x, unsigned int y)
{
	isRButtonDown = true;

	if (frameStateMgr)
	{
		frameStateMgr->onRButtonUp(x, y);
	}
}

void I4MiniGameFrameCallback::onRButtonUp(unsigned int x, unsigned int y)
{
	isRButtonDown = false;

	if (frameStateMgr)
	{
		frameStateMgr->onRButtonUp(x, y);
	}
}

void I4MiniGameFrameCallback::updateCamera(float deltaTime)
{
	I4Framework* framework = I4Framework::getFramework();

	float camMoveSpeed = 6.0f*deltaTime;
	I4Vector3 newCamEye = camera->getEye();
	I4Quaternion newCamRotation = camera->getRotation();

	if (framework->isKeyPressed('w') || framework->isKeyPressed('W'))
	{
		newCamEye += camera->getDirection()*camMoveSpeed;
	}

	if (framework->isKeyPressed('s') || framework->isKeyPressed('S'))
	{
		newCamEye -= camera->getDirection()*camMoveSpeed;
	}

	if (framework->isKeyPressed('a') || framework->isKeyPressed('A'))
	{
		newCamEye -= camera->getRight()*camMoveSpeed;
	}

	if (framework->isKeyPressed('d') || framework->isKeyPressed('D'))
	{
		newCamEye += camera->getRight()*camMoveSpeed;
	}

	if (isRButtonDown)
	{
		POINT pt;
		GetCursorPos(&pt);

		int curMouseX = 0;
		int curMouseY = 0;

		framework->getMousePos(curMouseX, curMouseY);

		int dx = curMouseX - prevMouseX;
		int dy = curMouseY - prevMouseY;

		const float CAMERA_SENSITIVE = 0.35f;
		camYaw += dx*CAMERA_SENSITIVE;
		camPitch += dy*CAMERA_SENSITIVE;

		if (camYaw > 360)
		{
			camYaw -= 360;
		}

		if (camPitch < -90)
		{
			camPitch = -90;
		}
		
		if (camPitch > 90)
		{
			camPitch = 90;
		}


		newCamRotation.makeRotationYawPitchRoll(I4MathUtil::degreeToRadian(camYaw), I4MathUtil::degreeToRadian(camPitch), I4MathUtil::degreeToRadian(camRoll));
	}

	camera->setTransform(newCamRotation, newCamEye);

	
	framework->getMousePos(prevMouseX, prevMouseY);
}


void I4MiniGameFrameCallback::commitToRenderer(float deltaTime)
{
	I4PROFILE_THISFUNC;

	I4MessageArgs aniArgs;
	aniArgs.push_back(deltaTime);
	objectMgr->getMessenger().send(I4Hash("onAnimate"), aniArgs);

	I4MessageArgs renderArgs;
	objectMgr->getMessenger().send(I4Hash("onRender"), renderArgs);
	
	I4DirectionalLight directionalLight;
	directionalLight.direction = I4Vector3(0.7f, -1.0f, 0.75f);

	/*
		sunset=182,126,91
		noon=192,191,173
		clouds, haze=189,190,192
		overcast=174,183,190
	*/
	directionalLight.color = I4Vector3(192.0f/255.0f, 191.0f/255.0f, 173.0f/255.0f);

	renderer->commitToScene(&directionalLight);


	I4PointLight pointLight[200];
	
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

	static float degree = 0;
	degree += 30*deltaTime;
	if (degree > 360.0f)
	{
		degree -= 360.0f;
	}

	for (int i = 0; i < _countof(pointLight)/10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int sign = 1;
			if (i%2 == 1)
			{
				sign = -1;
			}

			int idx = i*10 + j;
			pointLight[idx].position.x = -20.0f + i*2.0f + sign*1.5f*cos(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].position.y = 1.0f;
			pointLight[idx].position.z = -20.0f + j*4.0f + sign*1.5f*sin(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].radius = 3.0f;
			pointLight[idx].color = lightPointColor[idx%13];

			renderer->commitToScene(&pointLight[idx]);
		}
	}
}