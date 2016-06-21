#pragma once
#define LIFE_NUM 30

#include "GameState.h"
struct AABB
{
	float left, right, top, bottom, back, front;
};
class PlayState : public GameState
{
public:
	void enter(void);
	void exit(void);

	void pause(void);
	void resume(void);

	bool colidelife(const AABB&);

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e);
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e);
	void setbox();
	static PlayState* getInstance() { return &mPlayState; }
	AABB lifebox[LIFE_NUM];
	AABB playerbox;
private:

	void _setLights(void);
	void _drawGroundPlane(void);
	void _drawGridPlane(void);
	float mObjectSpeed;

	static PlayState mPlayState;
	Ogre::Vector3 mCharacterSpeed;
	Ogre::Root *mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;

	Ogre::Light *mLightP, *mLightD, *mLightS;

	Ogre::SceneNode* mCharacterRoot;
	Ogre::SceneNode* mGroundRoot[2];
	Ogre::SceneNode* mLifeRoot[LIFE_NUM];
	Ogre::SceneNode* mCharacterYaw;
	Ogre::SceneNode* mCameraHolder;
	Ogre::SceneNode* mCameraYaw;
	Ogre::SceneNode* mCameraPitch;
	Ogre::Entity* mCharacterEntity;
	Ogre::Entity* mLifeEntity;
	Ogre::AnimationState* mAnimationState;

	Ogre::Overlay*           mInformationOverlay;

};


