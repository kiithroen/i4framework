#include "stdafx.h"
#include "I4MiniGameFrameCallback.h"
#include "I4MiniGameState_Game.h"
#include "I4Framework.h"
#include "I4DefferedRenderer.h"
#include "I4Camera.h"
#include "I4FrameStateMgr.h"
#include "I4Log.h"
#include "I4Profile.h"
#include "I4ProfileWriterLog.h"
#include "I4Actor.h"
#include "I4ActorMgr.h"

using namespace i4graphics;

I4MiniGameFrameCallback::I4MiniGameFrameCallback()
: frameStateMgr(nullptr)
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
	renderer = new I4DefferedRenderer;
	if (renderer->initialize(framework->getWindowID(), framework->getWidth(), framework->getHeight()) == false)
	{
		I4LOG_ERROR << "renderer initalize failed.";
		return false;
	}
	
	camera = new I4Camera;
	camera->setPerspectiveFov(PI/4.0f, (float)framework->getWidth()/(float)framework->getHeight(), 1.0f, 1000.0f);
	camera->setLookAt(I4Vector3(8.0f, 2.0f, -20.0f), I4Vector3(-2.0f, 0.0f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));
	
	float camYawRad;
	float camPitchRad;
	float camRollRad;
	camera->getRotation().extractYawPitchRoll(camYawRad, camPitchRad, camRollRad);

	camYaw = I4MathUtil::radianToDegree(camYawRad);
	camPitch = I4MathUtil::radianToDegree(camPitchRad);
	camRoll = I4MathUtil::radianToDegree(camRollRad);

	frameStateMgr = new I4FrameStateMgr;
	frameStateMgr->addFrameState("minigame", new I4MiniGameState_Game);
	frameStateMgr->changeFrameState("minigame");

	framework->moveMouseCenter();
	framework->getMousePos(prevMouseX, prevMouseY);


	actorMgr = new I4ActorMgr;
	actor = actorMgr->createActor("actor");
	if (!actorMgr->attachBone(actor, "cyberdemon.bone.xml"))
	{
		return FALSE;
	}

	if (!actorMgr->attachMesh(actor, "cyberdemon.mesh.xml"))
	{
		return FALSE;
	}

	if (!actorMgr->attachAni(actor, "cyberdemon.ani.xml", "idle"))
	{
		return FALSE;
	}

	if (!actor->initialize())
	{
		return FALSE;
	}

	actor->playAnimation("idle");

	return true;
}

void I4MiniGameFrameCallback::onEnd()
{
	delete actor;
	delete actorMgr;
	delete renderer;
	delete camera;
	delete frameStateMgr;
}

bool I4MiniGameFrameCallback::onUpdate(float deltaSec)
{
	I4Framework* framework = I4Framework::getFramework();
	
	if (framework->isKeyPressed(VK_ESCAPE))
		return false;

	if (frameStateMgr == NULL)
		return true; 

	if (frameStateMgr->onUpdate(deltaSec) == false)
		return false;

	actor->animate(deltaSec);

	updateCamera(deltaSec);

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
	PROFILE_THISFUNC;

	I4Framework* framework = I4Framework::getFramework();

	float camMoveSpeed = 100.0f*deltaTime;
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
	PROFILE_THISFUNC;

	actor->render(renderer, IDENTITY);

	I4Matrix4x4 matModel;
	I4Matrix4x4 matScale;

	I4DirectionalLight directionalLight[2] =
	{
		{ I4Vector3(-1.0f, -1.0f, 1.0f), I4Vector3(1.0f, 1.0f, 1.0f) },
		{ I4Vector3(1.0f, 0.3f, 0.3f), I4Vector3(1.0f, 0.0f, 0.0f) },
	};

	for (int i = 0; i < _countof(directionalLight); ++i)
	{
		renderer->commitToScene(&directionalLight[i]);
	}

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
			pointLight[idx].position.x = -250.0f + i*25.0f + sign*45*cos(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].position.y = 10.0f;
			pointLight[idx].position.z = -250.0f + j*50.0f + sign*15*sin(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].radius = 25.0f;
			pointLight[idx].color = lightPointColor[idx%13];

			renderer->commitToScene(&pointLight[idx]);
		}
	}
}