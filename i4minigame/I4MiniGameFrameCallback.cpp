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
	renderer = new I4DeferredRenderer;
	if (renderer->initialize(framework->getWindowID(), framework->getWidth(), framework->getHeight()) == false)
	{
		I4LOG_ERROR << "renderer initalize failed.";
		return false;
	}
	
	camera = new I4Camera;
	camera->setPerspectiveFov(I4PI/4.0f, (float)framework->getWidth()/(float)framework->getHeight(), 0.1f, 50.0f);
	camera->setLookAt(I4Vector3(0.0f, 2.5f, -1.0f), I4Vector3(0.0f, 2.2f, 0.0f), I4Vector3(0.0f, 1.0f, 0.0f));
	
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
	
	floor = actorMgr->createActor("floor");
	if (!actorMgr->attachMesh(floor, "testactor/floor.mesh.xml"))
	{
		return false;
	}

	floor->setShadowCaster(false);

	for (int i = 0; i < 100; ++i)
	{
		char buf[128] = {0, };
		_itoa_s(i, buf, 10);

		actor[i] = actorMgr->createActor("actor_" + string(buf));
		if (i%3 == 0)
		{
			if (!actorMgr->attachBone(actor[i], "testactor/cyberdemon.bone.xml"))
			{
				return false;
			}

			if (!actorMgr->attachMesh(actor[i], "testactor/cyberdemon.mesh.xml"))
			{
				return false;
			}

			if (!actorMgr->attachMaterial(actor[i], "testactor/cyberdemon.mtrl.xml"))
			{
				return false;
			}

			if (!actorMgr->attachAni(actor[i], "testactor/cyberdemon.ani.xml", "idle"))
			{
				return false;
			}

			if (!actor[i]->initialize())
			{
				return false;
			}

			actor[i]->playAnimation("idle");
		}
		else if (i%3 == 1)
		{
			if (!actorMgr->attachBone(actor[i], "testactor/guard.bone.xml"))
			{
				return false;
			}

			if (!actorMgr->attachMesh(actor[i], "testactor/guard.mesh.xml"))
			{
				return false;
			}

			if (!actorMgr->attachMaterial(actor[i], "testactor/guard.mtrl.xml"))
			{
				return false;
			}

			if (!actorMgr->attachAni(actor[i], "testactor/guard_idle.ani.xml", "idle"))
			{
				return false;
			}

			if (!actor[i]->initialize())
			{
				return false;
			}

			actor[i]->playAnimation("idle");
		}
		else
		{
			if (!actorMgr->attachMesh(actor[i], "testactor/elin.mesh.xml"))
			{
				return false;
			}

			if (!actorMgr->attachMaterial(actor[i], "testactor/elin.mtrl.xml"))
			{
				return false;
			}

			if (!actor[i]->initialize())
			{
				return false;
			}
		}
	}
	

	return true;
}

void I4MiniGameFrameCallback::onEnd()
{
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

	static float angle = 0;

	//angle += 15*deltaTime;

	if (angle > 360)
	{
		angle = 0;
	}

	I4Matrix4x4 matR;
	matR.makeRotationY(I4MathUtil::degreeToRadian(angle));


	I4Matrix4x4 matT;

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			int idx = i*10 + j;
			matT.makeTranslation(-20.0f + i*4.0f, 0.0f, -20.0f + j*4.0f);

			if (i%2 == 0)
			{
				actor[idx]->animate(deltaTime*2);
			}
			else
			{
				actor[idx]->animate(deltaTime);
			}

			if (idx%3 == 0)	// cyberdemon
			{
				I4Matrix4x4 matS;
				matS.makeScale(0.006f, 0.006f, 0.006f);
				actor[idx]->render(renderer, matS*matR*matT);
			}
			else if (idx%3 == 1)	// guard
			{				
				I4Matrix4x4 matS;
				matS.makeScale(0.01f, 0.01f, 0.01f);
				actor[idx]->render(renderer, matS*matT);
			}
			else	// elin
			{
				I4Matrix4x4 matS;
				matS.makeScale(0.05f, 0.05f, 0.05f);
				actor[idx]->render(renderer, matS*matR*matT);
			}
		}
	}

	floor->animate(deltaTime);

	I4Matrix4x4 matS;
	matS.makeScale(1, 1, 1);
	floor->render(renderer, matS);

	I4DirectionalLight directionalLight[] =
	{
		{ I4Vector3(0.5f, -1.0f, 0.5f), I4Vector3(0.9f, 0.9f, 0.9f) },
//		{ I4Vector3(1.0f, 3.0f, -1.0f), I4Vector3(0.7f, 0.3f, 0.2f) },
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
			pointLight[idx].position.x = -20.0f + i*2.0f + sign*1.5f*cos(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].position.y = 1.0f;
			pointLight[idx].position.z = -20.0f + j*4.0f + sign*1.5f*sin(I4MathUtil::degreeToRadian(degree));
			pointLight[idx].radius = 3.0f;
			pointLight[idx].color = lightPointColor[idx%13];

			renderer->commitToScene(&pointLight[idx]);
		}
	}
}