#include "PlayState.h"
#include "TitleState.h"
#include <iostream>
#include <string>
using namespace Ogre;
using namespace std;
PlayState PlayState::mPlayState;




void PlayState::enter(void)
{
	srand(time(0));
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");
	mCamera->setPosition(Ogre::Vector3::ZERO);

	_drawGridPlane();
	_setLights();
	mSceneMgr->setSkyBox(true, "Sky/PlaySkyBox", 5000);
	_drawGroundPlane();

	/*mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();*/

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
	mCharacterDirection = Vector3(0,0,-1);
	char c = 'A';
	string s = "Rock";
	for (int i = 0; i < 1; ++i)
	{
		Vector3 v(rand() % 951 - 450, rand() % 501, -1000.f);
		mRockRoot[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(s + c, v);
		mRockEntity = mSceneMgr->createEntity(s+c, "outline.mesh");
		mRockEntity->setCastShadows(false);

		mRockRoot[i]->attachObject(mRockEntity);
		mRockRoot[i]->setScale(Vector3(100, 100, 100));

		lifebox[i].left = v.x - 30;
		lifebox[i].right = v.x + 30;
		lifebox[i].top = v.y + 40;
		lifebox[i].bottom = v.y;
		lifebox[i].front = v.z + 10;
		lifebox[i].back = v.z - 10;
		c++;
	}
	
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	//mInformationOverlay->hide();
	
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
	static int i = 0;
	if (mCharacterDirection!=Vector3::ZERO)
		mCharacterRoot->translate(mCharacterDirection.normalisedCopy() * 111 * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);
	if (colidelife())
		cout << i++<< " collision" << endl;
	
	switch (state)
	{
	case RIGHT:
		mCharacterDirection.x = 10;
		break;
	case LEFT:
		mCharacterDirection.x = -10;
		break;
	case IDLE:
		mCharacterDirection.x = 0;
		break;
	}

	switch (state2)
	{
	case UP:
		mCharacterDirection.y = 10;
		break;
	case DOWN:
		mCharacterDirection.y = -10;
		break;
	case IDLE2:
		mCharacterDirection.y = 0;
		break;
	}
	mAnimationState->addTime(evt.timeSinceLastFrame);
	playerbox.right = mCharacterRoot->getPosition().x + 30;
	playerbox.left = mCharacterRoot->getPosition().x - 30;
	playerbox.top = mCharacterRoot->getPosition().y + 150;
	playerbox.bottom = mCharacterRoot->getPosition().y;
	playerbox.front = mCharacterRoot->getPosition().z + 10;
	playerbox.back = mCharacterRoot->getPosition().z - 10;

	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	/*static Ogre::DisplayString currFps = L"현재 FPS: ";
	static Ogre::DisplayString avgFps = L"평균 FPS: ";
	static Ogre::DisplayString bestFps = L"최고 FPS: ";
	static Ogre::DisplayString worstFps = L"최저 FPS: ";

	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));*/

	return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch (e.key)
	{
	case OIS::KC_A:
		if (state==LEFT)
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
		mCharacterDirection.z -= 10;
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
		mCharacterDirection.z+= 10;
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
		1000, 5000,
		1, 1,
		true, 1, 5, 5,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);

	groundEntity->setMaterialName("KPU_LOGO2");
	groundEntity->setCastShadows(false);
}

void PlayState::_drawGridPlane(void)
{
	// 좌표축 표시
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

	Ogre::ManualObject* gridPlane = mSceneMgr->createManualObject("GridPlane");
	Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode");

	Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial", "General");
	gridPlaneMaterial->setReceiveShadows(false);
	gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 0);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1, 1, 1);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1, 1, 1);

	gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i<21; i++)
	{
		gridPlane->position(-500.0f, 0.0f, 500.0f - i * 50);
		gridPlane->position(500.0f, 0.0f, 500.0f - i * 50);

		gridPlane->position(-500.f + i * 50, 0.f, 500.0f);
		gridPlane->position(-500.f + i * 50, 0.f, -500.f);
	}

	gridPlane->end();

	gridPlaneNode->attachObject(gridPlane);
}

bool PlayState::colidelife()
{
	for (int i = 0; i < 1; ++i)
	{
		if (lifebox[i].left > playerbox.right)
			return false;
		if (lifebox[i].right < playerbox.left)
			return false;
		if (lifebox[i].top < playerbox.bottom)
			return false;
		if (lifebox[i].bottom > playerbox.top)
			return false;
		if (lifebox[i].front < playerbox.back)
			return false;
		if (lifebox[i].back > playerbox.front)
			return false;
		return true;
	}
}