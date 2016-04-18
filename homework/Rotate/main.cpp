#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>
#include <math.h>

using namespace Ogre;


class ESCListener : public FrameListener {
	OIS::Keyboard *mKeyboard;

public:
	ESCListener(OIS::Keyboard *keyboard) : mKeyboard(keyboard) {}
  bool frameStarted(const FrameEvent &evt)
  {
    mKeyboard->capture();
	return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
  }
};


class MainListener : public FrameListener {
  OIS::Keyboard *mKeyboard;
  Root* mRoot;
  SceneNode *mProfessorNode, *mFishManagerNode, *mFishNode;

public:
  MainListener(Root* root, OIS::Keyboard *keyboard) : mKeyboard(keyboard), mRoot(root) 
  {
    mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");
    mFishNode = mRoot->getSceneManager("main")->getSceneNode("Ninja");
	mFishManagerNode = mRoot->getSceneManager("main")->getSceneNode("manager");
  }

  bool frameStarted(const FrameEvent &evt)
  {
	  // Fill Here ----------------------------------------------
	  static SceneNode* professorNode = mProfessorNode;
	  static SceneNode* fishNode = mFishNode;
	  static SceneNode* fishManagerNode = mFishManagerNode;

	  const float professorVelocityZ = 250.f;
	  const float fishAngularVelocity = -250.f;
	  static int professorRotateDegree = 0;
	  static bool professorTurning = true;
	  static int professorDirectionZ = 1;
	  float frameTime = evt.timeSinceLastFrame;

	  professorNode->translate(0, 0, professorDirectionZ*frameTime*professorVelocityZ);
	  if (professorNode->getPosition().z >= 250.f)
	  {
		  professorNode->setPosition(Vector3(0, 0, 250.f));
		  professorDirectionZ = 0;
		  professorNode->yaw(Degree(1.0f));
		  professorRotateDegree++;
		  if (professorRotateDegree == 180)
		  {
			  professorDirectionZ = -1;
			  professorRotateDegree = 0;
		  }
	  }
	  if (professorNode->getPosition().z <= -250.f)
	  {
		  professorNode->setPosition(Vector3(0, 0, -250.f));
		  professorDirectionZ = 0;
		  professorNode->yaw(Degree(1.0f));
		  professorRotateDegree++;
		  if (professorRotateDegree == 180)
		  {
			  professorDirectionZ = 1;
			  professorRotateDegree = 0;
		  }
	  }
	  fishManagerNode->setInheritOrientation(false);
	  fishManagerNode->yaw(Degree(fishAngularVelocity*frameTime));
    // --------------------------------------------------------

    return true;
  }

};

class LectureApp {

  Root* mRoot;
  RenderWindow* mWindow;
  SceneManager* mSceneMgr;
  Camera* mCamera;
  Viewport* mViewport;
  OIS::Keyboard* mKeyboard;
  OIS::InputManager *mInputManager;

  MainListener* mMainListener;
  ESCListener* mESCListener;

public:

  LectureApp() {}

  ~LectureApp() {}

  void go(void)
  {
    // OGRE�� ���� ��Ʈ ������Ʈ ����
#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
    mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


    // �ʱ� ������ ���ǱԷ��̼� ���� - ogre.cfg �̿�
    if (!mRoot->restoreConfig()) {
      if (!mRoot->showConfigDialog()) return;
    }

    mWindow = mRoot->initialise(true, "Rotate : Copyleft by Dae-Hyun Lee");


    // ESC key�� ������ ���, ����� ���� ������ ������ Ż���� ó��
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList pl;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mInputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));


    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
    mCamera = mSceneMgr->createCamera("main");


    mCamera->setPosition(0.0f, 100.0f, 700.0f);
    mCamera->lookAt(0.0f, 100.0f, 0.0f);

    mCamera->setNearClipDistance(5.0f);

    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.5f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));


    ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	ResourceGroupManager::getSingleton().addResourceLocation("fish.zip", "Zip");
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

    // ��ǥ�� ǥ��
    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

    _drawGridPlane();


    Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
    SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor", Vector3(0.0f, 0.0f, 0.0f));
    node1->attachObject(entity1);

	SceneNode *managerNode = node1->createChildSceneNode("manager", Vector3(0, 0, 0));

    Entity* entity2 = mSceneMgr->createEntity("Ninja", "fish.mesh");
    SceneNode* node2 = managerNode->createChildSceneNode("Ninja", Vector3(100.0f, 0.0f, 0.0f));
	node2->scale(Vector3(10, 10, 10));
    node2->attachObject(entity2);
	node2->yaw(Degree(60.f));


    mESCListener =new ESCListener(mKeyboard);
    mRoot->addFrameListener(mESCListener);

    mMainListener = new MainListener(mRoot, mKeyboard);
    mRoot->addFrameListener(mMainListener);


    mRoot->startRendering();

    mInputManager->destroyInputObject(mKeyboard);
    OIS::InputManager::destroyInputSystem(mInputManager);

    delete mRoot;
  }

private:
  void _drawGridPlane(void)
  {
    Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
    Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

    Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
    gridPlaneMaterial->setReceiveShadows(false); 
    gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

    gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
    for(int i=0; i<21; i++)
    {
      gridPlane->position(-500.0f, 0.0f, 500.0f-i*50);
      gridPlane->position(500.0f, 0.0f, 500.0f-i*50);

      gridPlane->position(-500.f+i*50, 0.f, 500.0f);
      gridPlane->position(-500.f+i*50, 0.f, -500.f);
    }

    gridPlane->end(); 

    gridPlaneNode->attachObject(gridPlane);
  }
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    LectureApp app;

    try {

      app.go();

    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
        e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
  }

#ifdef __cplusplus
}
#endif

