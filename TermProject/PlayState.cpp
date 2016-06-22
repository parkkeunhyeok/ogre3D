#include "PlayState.h"
#include "TitleState.h"
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <iostream>
#include <string>
#include <mmsystem.h>
using namespace Ogre;
using namespace std;
PlayState PlayState::mPlayState;



void PlayState::enter(void)
{
	PlaySound("sound/2014.wav", NULL, SND_ASYNC);
	mCharacterSpeed = { 700, 700, 200 };
	mGroundDirection = { 0, 0, 1 };
	mObjectSpeed = { 600, 600, 1000 };
	mLife = 100;
	mScore = 0;
	srand(time(0));
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();
	
	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");
	mCamera->setPosition(Ogre::Vector3::ZERO);
	
	_setLights();
	mSceneMgr->setSkyBox(true, "Sky/PlaySkyBox", 5000);
	_drawGroundPlane();

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");

	mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 120.0f, 0.0f));
	mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
	mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 180.0f, 500.0f));

	mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mCharacterYaw->attachObject(mCharacterEntity);
	mCharacterEntity->setCastShadows(true);

	mCameraHolder->attachObject(mCamera);
	mCamera->lookAt(mCameraYaw->getPosition());

	mAnimationState = mCharacterEntity->getAnimationState("Run");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	mCharacterYaw->yaw(Degree(180.f));
	mCharacterDirection = Vector3(0, 0, 0);
	
	char c = 'A';
	string s = "Life";
	for (int i = 0; i < LIFE_NUM; ++i)
	{
		int a = rand() % 21 - 10;
		int b = rand() % 21 - 10;
		mObjectDirection[i] = Vector3(a*0.1, b*0.1, 1);
		Vector3 v(rand() % 900 - 450, 100, -1000.f);
		mLifeRoot[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(s + c, v);
		mLifeEntity = mSceneMgr->createEntity(s + c, "outline.mesh");
		mLifeEntity->setCastShadows(false);

		mLifeRoot[i]->attachObject(mLifeEntity);
		mLifeRoot[i]->translate(Vector3(0, 0, -(i * 300)));
		mLifeRoot[i]->setScale(Vector3(100, 100, 100));
		lifebox[i].left = v.x - 30;
		lifebox[i].right = v.x + 30;
		lifebox[i].top = v.y + 40;
		lifebox[i].bottom = v.y;
		lifebox[i].front = v.z + 10;
		lifebox[i].back = v.z - 10;
		c++;
	}

}

void PlayState::collidelifewall()
{
	for (int i = 0; i < LIFE_NUM; ++i)
	{

		if (mLifeRoot[i]->getPosition().x < -500)
		{
			mLifeRoot[i]->setPosition(-500, mLifeRoot[i]->getPosition().y, mLifeRoot[i]->getPosition().z);
			mObjectDirection[i] = mObjectDirection[i].reflect(Vector3(1, 0, 0));
		}
		if (mLifeRoot[i]->getPosition().x > 500)
		{
			mLifeRoot[i]->setPosition(500, mLifeRoot[i]->getPosition().y, mLifeRoot[i]->getPosition().z);
			mObjectDirection[i] = mObjectDirection[i].reflect(Vector3(-1, 0, 0));
		}
		if (mLifeRoot[i]->getPosition().y < -50)
		{
			mLifeRoot[i]->setPosition(mLifeRoot[i]->getPosition().x, -50, mLifeRoot[i]->getPosition().z);
			mObjectDirection[i] = mObjectDirection[i].reflect(Vector3(0, -1, 0));
		}
		if (mLifeRoot[i]->getPosition().y > 500)
		{
			mLifeRoot[i]->setPosition(mLifeRoot[i]->getPosition().x, 500, mLifeRoot[i]->getPosition().z);
			mObjectDirection[i] = mObjectDirection[i].reflect(Vector3(0, -1, 0));
		}
	}
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();

	// ---------------------------------------
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
	mAnimationState = mCharacterEntity->getAnimationState("Run");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	mScore += mObjectSpeed.z*0.01;
	collidelifewall();
	if (mCharacterDirection != Vector3::ZERO)
	{	
		
		mCharacterRoot->translate(mCharacterDirection.normalisedCopy() * mCharacterSpeed * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
	}
	if (mCharacterRoot->getPosition().x > 600)
		mCharacterRoot->setPosition(600, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCharacterRoot->getPosition().x < -600)
		mCharacterRoot->setPosition(-600, mCharacterRoot->getPosition().y, mCharacterRoot->getPosition().z);
	if (mCharacterRoot->getPosition().y > 500)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, 500, mCharacterRoot->getPosition().z);
	if (mCharacterRoot->getPosition().y < -50)
		mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, -50, mCharacterRoot->getPosition().z);
	for (int i = 0; i < LIFE_NUM; ++i)
	{
		if (mObjectDirection[i] != Vector3::ZERO)
			mLifeRoot[i]->translate(mObjectDirection[i].normalisedCopy() * mObjectSpeed * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
		if (mLifeRoot[i]->getPosition().z >= 200)
			mLifeRoot[i]->translate(0, 0, -9999);
		if (collidelife(lifebox[i]))
		{
			mLifeRoot[i]->translate(0, 0, -3200);
			mLife++;
			if (mLife > 100)
				mLife = 100;
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		if (mGroundDirection != Vector3::ZERO)
			mGroundRoot[i]->translate(mGroundDirection.normalisedCopy() *mObjectSpeed* evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
		if (mGroundRoot[i]->getPosition().z >= 10000)
			mGroundRoot[i]->setPosition(Vector3(0, 0, -9500));
	}
	

	switch (state)
	{
	case RIGHT:
		mCharacterDirection.x = 1;
		break;
	case LEFT:
		mCharacterDirection.x = -1;
		break;
	case IDLE:
		mCharacterDirection.x = 0;
		break;
	}

	switch (state2)
	{
	case UP:
		mCharacterDirection.y = 1;
		break;
	case DOWN:
		mCharacterDirection.y = -1;
		break;
	case IDLE2:
		mCharacterDirection.y = 0;
		break;
	}
	mAnimationState->addTime(evt.timeSinceLastFrame);
	

	setbox();
	static float t;
	t += evt.timeSinceLastFrame;
	if (t > 1)
	{

		mLife -= 1 * mObjectSpeed.z*0.001;
		if (mLife <= 0)
		{
			mLife = 0;
			game->changeState(TitleState::getInstance());

		}
		mObjectSpeed.z += 100;
		t = 0;
	}
	return true;
}
void PlayState::setbox()
{
	for (int i = 0; i < LIFE_NUM; ++i)
	{
		lifebox[i].left = mLifeRoot[i]->getPosition().x - 30;
		lifebox[i].right = mLifeRoot[i]->getPosition().x + 30;
		lifebox[i].top = mLifeRoot[i]->getPosition().y + 40;
		lifebox[i].bottom = mLifeRoot[i]->getPosition().y;
		lifebox[i].front = mLifeRoot[i]->getPosition().z + 10;
		lifebox[i].back = mLifeRoot[i]->getPosition().z - 10;
	}

	playerbox.right = mCharacterRoot->getPosition().x + 30;
	playerbox.left = mCharacterRoot->getPosition().x - 30;
	playerbox.top = mCharacterRoot->getPosition().y + 150;
	playerbox.bottom = mCharacterRoot->getPosition().y;
	playerbox.front = mCharacterRoot->getPosition().z + 10;
	playerbox.back = mCharacterRoot->getPosition().z - 10;
}
bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{

	static Ogre::DisplayString score = L"SCORE : ";
	OverlayElement* Score = OverlayManager::getSingleton().getOverlayElement("Score");

	

	Score->setCaption(score + StringConverter::toString(mScore));

	static Ogre::DisplayString life = L"LIFE : ";
	OverlayElement* Life = OverlayManager::getSingleton().getOverlayElement("Life");


	Life->setCaption(life + StringConverter::toString(mLife));

	return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch (e.key)
	{
	case OIS::KC_A:
		if (state == LEFT)
			state = IDLE;
		break;
	case OIS::KC_D:
		if (state == RIGHT)
			state = IDLE;
		break;
	case OIS::KC_W:
		if (state2 == UP)
			state2 = IDLE2;
		break;
	case OIS::KC_S:
		if (state2 == DOWN)
			state2 = IDLE2;
		break;
	case OIS::KC_O:
		//mObjectDirection.z = 1;
		break;
	}
	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		game->changeState(TitleState::getInstance());
		break;
	case OIS::KC_A:
		state = LEFT;
		break;
	case OIS::KC_D:
		state = RIGHT;
		break;
	case OIS::KC_W:
		state2 = UP;
		break;
	case OIS::KC_S:
		state2 = DOWN;
		break;
	case OIS::KC_O:
		//mObjectDirection.z = -1;
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	/*mCameraYaw->yaw(Degree(-e.state.X.rel));
	mCameraPitch->pitch(Degree(-e.state.Y.rel));

	mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));*/
	return true;
}



void PlayState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -2.0f, -1));
	mLightD->setVisible(true);
}

void PlayState::_drawGroundPlane(void)
{
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		1000, 10000,
		1, 1,
		true, 1, 2, 2,
		Vector3::NEGATIVE_UNIT_Z
		);

	Plane plane2(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground2",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane2,
		1000, 10000,
		1, 1,
		true, 1, 5, 5,
		Vector3::NEGATIVE_UNIT_Z
		);
	mGroundRoot[0] = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ground", Vector3(0,0,0));


	Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
	groundEntity->setMaterialName("KPU_LOGO2");
	groundEntity->setCastShadows(false);

	mGroundRoot[0]->attachObject(groundEntity);

	mGroundRoot[1] = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ground2", Vector3(0, 0, -10000));


	Entity* groundEntity2 = mSceneMgr->createEntity("GroundPlane2", "Ground2");
	groundEntity2->setMaterialName("KPU_LOGO2");
	groundEntity2->setCastShadows(false);

	mGroundRoot[1]->attachObject(groundEntity2);

}

bool PlayState::collidelife(const AABB& box)
{
	
	if (box.left > playerbox.right)
			return false;
	if (box.right < playerbox.left)
			return false;
	if (box.top < playerbox.bottom)
			return false;
	if (box.bottom > playerbox.top)
			return false;
	if (box.front < playerbox.back)
			return false;
	if (box.back > playerbox.front)
			return false;
		return true;
	
}