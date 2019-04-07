#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
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
 
protected:
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
 
private:
  void createCamera();
  void createViewports();
  void defineTerrain(long x, long y);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
  void createBulletSim(void);
  void handleCamCollision(void);
  bool mTerrainsImported;

  Ogre::TerrainGroup* mTerrainGroup;
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
  OgreBites::Label* mInfoLabel;
  Ogre::ManualObject* object;
  Ogre::MeshPtr pMeshPtr;

  btDefaultCollisionConfiguration* collisionConfiguration;
  btCollisionDispatcher* dispatcher;
  btBroadphaseInterface* overlappingPairCache;
  btSequentialImpulseConstraintSolver* solver;
  btDiscreteDynamicsWorld* dynamicsWorld;
  btCollisionShape* groundShape;
  btAlignedObjectArray<btCollisionShape*> collisionShapes;
};
