// Modified copy of OgreBites::SdkCameraMan.cpp.
// This version moves a node the camera is attached to rather than the camera itself.

#include "CameraMan.h"

using namespace CameraController;

//-------------------------------------------------------------------------------------
CameraMan::CameraMan(Ogre::Camera* cam, Ogre::SceneNode* node, Ogre::SceneNode* yawNode, Ogre::SceneNode* pitchNode, Ogre::SceneNode* rollNode)
	: mCamera(0)
	, mCamNode(node)
	, mCamYawNode(yawNode)
	, mCamPitchNode(pitchNode)
	, mCamRollNode(rollNode)
	, mTarget(0)
	, mOrbiting(false)
	, mZooming(false)
	, mTopSpeed(150)
	, mVelocity(Ogre::Vector3::ZERO)
	, mGoingForward(false)
	, mGoingBack(false)
	, mGoingLeft(false)
	, mGoingRight(false)
	, mGoingUp(false)
	, mGoingDown(false)
	, mFastMove(false)
{
	setCamera(cam);
	mCamNode = node;
	mCamYawNode = yawNode;
	mCamPitchNode = pitchNode;
	mCamRollNode = rollNode;
	setStyle(CS_FREELOOK);
}

CameraMan::~CameraMan()
{

}

/*-----------------------------------------------------------------------------
| Swaps the camera on our camera man for another camera.
-----------------------------------------------------------------------------*/
void CameraMan::setCamera(Ogre::Camera* cam)
{
	mCamera = cam;
}

Ogre::Camera* CameraMan::getCamera()
{
	return mCamera;
}

/*-----------------------------------------------------------------------------
| Sets the target we will revolve around. Only applies for orbit style.
-----------------------------------------------------------------------------*/
void CameraMan::setTarget(Ogre::SceneNode* target)
{
	if (target != mTarget)
	{
		mTarget = target;
		if (target)
		{
			setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
			mCamera->setAutoTracking(true, mTarget);
		}
		else
		{
			mCamera->setAutoTracking(false);
		}

	}


}

Ogre::SceneNode* CameraMan::getTarget()
{
	return mTarget;
}

/*-----------------------------------------------------------------------------
| Sets the spatial offset from the target. Only applies for orbit style.
-----------------------------------------------------------------------------*/
void CameraMan::setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist)
{
	mCamNode->setPosition(mTarget->_getDerivedPosition());
	mCamNode->setOrientation(mTarget->_getDerivedOrientation());
	mCamNode->yaw(yaw);
	mCamNode->pitch(-pitch);
	mCamNode->translate(Ogre::Vector3(0, 0, dist));
}

/*-----------------------------------------------------------------------------
| Sets the camera's top speed. Only applies for free-look style.
-----------------------------------------------------------------------------*/
void CameraMan::setTopSpeed(Ogre::Real topSpeed)
{
	mTopSpeed = topSpeed;
}

Ogre::Real CameraMan::getTopSpeed()
{
	return mTopSpeed;
}

/*-----------------------------------------------------------------------------
| Sets the movement style of our camera man.
-----------------------------------------------------------------------------*/
void CameraMan::setStyle(CameraStyle style)
{
	if (mStyle != CS_ORBIT && style == CS_ORBIT)
	{
		setTarget(mTarget ? mTarget : mCamera->getSceneManager()->getRootSceneNode());
		mCamera->setFixedYawAxis(true);
		manualStop();
		setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
	}
	else if (mStyle != CS_FREELOOK && style == CS_FREELOOK)
	{
		mCamera->setAutoTracking(false);
		mCamera->setFixedYawAxis(true);
	}
	else if (mStyle != CS_MANUAL && style == CS_MANUAL)
	{
		mCamera->setAutoTracking(false);
		manualStop();
	}
	mStyle = style;

}

CameraStyle CameraMan::getStyle()
{
	return mStyle;
}

/*-----------------------------------------------------------------------------
| Manually stops the camera when in free-look mode.
-----------------------------------------------------------------------------*/
void CameraMan::manualStop()
{
	if (mStyle == CS_FREELOOK)
	{
		mGoingForward = false;
		mGoingBack = false;
		mGoingLeft = false;
		mGoingRight = false;
		mGoingUp = false;
		mGoingDown = false;
		mVelocity = Ogre::Vector3::ZERO;
	}
}

bool CameraMan::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mStyle == CS_FREELOOK)
	{
		// build our acceleration vector based on keyboard input composite
		Ogre::Vector3 accel = Ogre::Vector3::ZERO;
		if (mGoingForward) accel += mCamera->getDirection();
		if (mGoingBack) accel -= mCamera->getDirection();
		if (mGoingRight) accel += mCamera->getRight();
		if (mGoingLeft) accel -= mCamera->getRight();
		if (mGoingUp) accel += mCamera->getUp();
		if (mGoingDown) accel -= mCamera->getUp();

		// if accelerating, try to reach top speed in a certain time
		Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
		if (accel.squaredLength() != 0)
		{
			accel.normalise();
			mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
		}
		// if not accelerating, try to stop in a certain time
		else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

		Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

		// keep camera velocity below top speed and above epsilon
		if (mVelocity.squaredLength() > topSpeed * topSpeed)
		{
			mVelocity.normalise();
			mVelocity *= topSpeed;
		}
		else if (mVelocity.squaredLength() < tooSmall * tooSmall)
			mVelocity = Ogre::Vector3::ZERO;

		if (mVelocity != Ogre::Vector3::ZERO)
			mCamNode->translate(
				this->mCamYawNode->getOrientation() *
				this->mCamPitchNode->getOrientation() *
				mVelocity * evt.timeSinceLastFrame,
				Ogre::SceneNode::TS_LOCAL
			);
	}

	return true;
}

/*-----------------------------------------------------------------------------
| Processes key presses for free-look style movement.
-----------------------------------------------------------------------------*/
void CameraMan::injectKeyDown(const OIS::KeyEvent& evt)
{
	if (mStyle == CS_FREELOOK)
	{
		if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
		else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
		else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
		else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
		else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
		else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
		else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
	}
}

/*-----------------------------------------------------------------------------
| Processes key releases for free-look style movement.
-----------------------------------------------------------------------------*/
void CameraMan::injectKeyUp(const OIS::KeyEvent& evt)
{
	if (mStyle == CS_FREELOOK)
	{
		if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false;
		else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
		else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
		else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
		else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
		else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
		else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
	}
}

/*-----------------------------------------------------------------------------
| Processes mouse movement differently for each style.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseMove(const OIS::MultiTouchEvent& evt)
#else
void CameraMan::injectMouseMove(const OIS::MouseEvent& evt)
#endif
{
	if (mStyle == CS_ORBIT)
	{
		Ogre::Real dist = (mCamNode->getPosition() - mTarget->_getDerivedPosition()).length();

		if (mOrbiting)   // yaw around the target, and pitch locally
		{
			mCamNode->setPosition(mTarget->_getDerivedPosition());

			mCamNode->yaw(Ogre::Degree(-evt.state.X.rel * 0.25f));
			mCamNode->pitch(Ogre::Degree(-evt.state.Y.rel * 0.25f));

			mCamNode->translate(Ogre::Vector3(0, 0, dist));

			// don't let the camera go over the top or around the bottom of the target
		}
		else if (mZooming)  // move the camera toward or away from the target
		{
			// the further the camera is, the faster it moves
			mCamNode->translate(Ogre::Vector3(0, 0, evt.state.Y.rel * 0.004f * dist));
		}
		else if (evt.state.Z.rel != 0)  // move the camera toward or away from the target
		{
			// the further the camera is, the faster it moves
			mCamNode->translate(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
		}
	}
	else if (mStyle == CS_FREELOOK)
	{
		this->mCamYawNode->yaw(Ogre::Degree(evt.state.X.rel * 0.15f));
		this->mCamPitchNode->pitch(Ogre::Degree(evt.state.Y.rel * 0.15f));
		
		Ogre::Real pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->mCamPitchNode->getOrientation().w)).valueDegrees());
		Ogre::Real pitchAngleSign = this->mCamPitchNode->getOrientation().x;
		if (pitchAngle > 90.0f) {
			if (pitchAngleSign > 0)
				this->mCamPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
			else if (pitchAngleSign < 0)
				this->mCamPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0));
		}
	}
}

/*-----------------------------------------------------------------------------
| Processes mouse presses. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseDown(const OIS::MultiTouchEvent& evt)
{
	if (mStyle == CS_ORBIT)
	{
		mOrbiting = true;
	}
}
#else
void CameraMan::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (mStyle == CS_ORBIT)
	{
		if (id == OIS::MB_Left) mOrbiting = true;
		else if (id == OIS::MB_Right) mZooming = true;
	}
}
#endif

/*-----------------------------------------------------------------------------
| Processes mouse releases. Only applies for orbit style.
| Left button is for orbiting, and right button is for zooming.
-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
void CameraMan::injectMouseUp(const OIS::MultiTouchEvent& evt)
{
	if (mStyle == CS_ORBIT)
	{
		mOrbiting = false;
	}
}
#else
void CameraMan::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (mStyle == CS_ORBIT)
	{
		if (id == OIS::MB_Left) mOrbiting = false;
		else if (id == OIS::MB_Right) mZooming = false;
	}
}
#endif