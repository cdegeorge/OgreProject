#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include "OgreManualObject.h"
#include "btBulletDynamicsCommon.h"
#include "btHeightfieldTerrainShape.h"
#include "BaseApplication.h"

class TutorialApplication : public BaseApplication
{
public:
  TutorialApplication();
  virtual ~TutorialApplication();
  Ogre::Vector3 playerPosition;
  unsigned long seconds;
  unsigned long minutes;
 
protected:
  CEGUI::OgreRenderer* mRenderer;
  CEGUI::Window* timer;
  virtual void createScene();
  virtual void createFrameListener();
  virtual void destroyScene();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& fe);
  bool frameStarted(const Ogre::FrameEvent &evt);
  void CreateCube(const btVector3 &Position, btScalar Mass, const btVector3 &scale, char * name);
  Ogre::ManualObject *createCubeMesh(Ogre::String name, Ogre::String matName);
  void CreatePlayer();
  Ogre::SceneNode* mPlayerNode;
  Ogre::Entity* mPlayerEntity;
  bool quit(const CEGUI::EventArgs &e);
 
private:
  void createCamera();
  void createViewports();
  void defineTerrain(long x, long y);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
  void createBulletSim(void);
  void handleCamCollision(void);
  void setTimeLimit(const unsigned long minutes);
  bool mTerrainsImported;
  unsigned long timeLimit;

  Ogre::TerrainGroup* mTerrainGroup;
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
  OgreBites::Label* mInfoLabel;
  Ogre::ManualObject* object;
  Ogre::MeshPtr pMeshPtr;
  Ogre::Timer mTimer;

  btDefaultCollisionConfiguration* collisionConfiguration;
  btCollisionDispatcher* dispatcher;
  btBroadphaseInterface* overlappingPairCache;
  btSequentialImpulseConstraintSolver* solver;
  btDiscreteDynamicsWorld* dynamicsWorld;
  btCollisionShape* groundShape;
  btAlignedObjectArray<btCollisionShape*> collisionShapes;
};
