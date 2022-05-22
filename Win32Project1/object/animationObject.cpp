#include "animationObject.h"
#include "../assets/assetManager.h"
#include "../node/node.h"
#include "../util/util.h"

AnimationObject::AnimationObject(Animation* anim) :Object() {
	animation = anim; // no mesh!
	anglex = 0; angley = 0; anglez = 0;
	setCurAnim("", false);
	setLoop(false);
	setPlayOnce(false);
	setMoving(false);
	setEnd(true);
	setDefaultAnim("");
	time = 0.0, curFrame = 0.0;
	initMatricesData();
	mass = 100.0;
	type = OBJ_TYPE_ANIMAT;
}

AnimationObject::AnimationObject(const AnimationObject& rhs) :Object(rhs) {
	animation=rhs.animation;
	if(rhs.bounding)
		bounding=rhs.bounding->clone();
	else
		bounding=NULL;
	anglex=rhs.anglex; angley=rhs.angley; anglez=rhs.anglez;
	fid = rhs.fid;

	position = rhs.position;
	size = rhs.size;
	localTransformMatrix = rhs.localTransformMatrix;
	normalMatrix = rhs.normalMatrix;

	rotateMat = rhs.rotateMat;
	translateMat = rhs.translateMat;
	scaleMat = rhs.scaleMat;

	genShadow = rhs.genShadow;
	detailLevel = rhs.detailLevel;
	time = rhs.time;
	curFrame = rhs.curFrame;
	loop = rhs.loop;
	playOnce = rhs.playOnce;
	moving = rhs.moving;
	animEnd = rhs.animEnd;
	defaultAname = rhs.defaultAname;

	if (rhs.transforms) {
		transforms = (float*)malloc(4 * sizeof(float));
		memcpy(transforms, rhs.transforms, 4 * sizeof(float));
	}
	if (rhs.transformsFull) {
		transformsFull = (buff*)malloc(16 * sizeof(buff));
		memcpy(transformsFull, rhs.transformsFull, 16 * sizeof(buff));
	}

	type = rhs.type;
}

AnimationObject::~AnimationObject() {

}

AnimationObject* AnimationObject::clone() {
	return new AnimationObject(*this);
}

void AnimationObject::vertexTransform() {
	translateMat = translate(position.x, position.y, position.z);
	rotateMat = Quat2Mat(rotateQuat);
	scaleMat = scale(size.x, size.y, size.z);

	localTransformMatrix = translateMat * rotateMat * scaleMat;
}

void AnimationObject::normalTransform() {
	if (size.x == size.y && size.y == size.z) {
		normalMatrix = localTransformMatrix;
		return;
	}
	normalMatrix = localTransformMatrix.GetInverse().GetTranspose();
}

void AnimationObject::setPosition(float x,float y,float z) {
	position.x=x;
	position.y=y;
	position.z=z;
	updateLocalMatrices();
}

void AnimationObject::setRotation(float ax, float ay, float az) {
	anglex = ax, angley = ay, anglez = az;
	RestrictAngle(anglex);
	RestrictAngle(angley);
	RestrictAngle(anglez);
	rotateQuat = Euler2Quat(vec3(anglex, angley, anglez));
	updateLocalMatrices();
}

void AnimationObject::setRotation(const vec4& q) {
	rotateQuat = q;
	updateLocalMatrices();
}

void AnimationObject::setSize(float sx, float sy, float sz) {
	size = vec3(sx, sy, sz);
	updateLocalMatrices();
	//if (billboard) { // todo in shader
	//	billboard->data[0] *= size.x;
	//	billboard->data[1] *= size.y;
	//}
}

bool AnimationObject::setCurAnim(const char* name, bool once) {
	aname = name;
	fid = AssetManager::assetManager->frames->frameIndex[name];
	setPlayOnce(once);
	return true;
}

void AnimationObject::animate(float velocity) {
	if (getCurAnim() == "") setCurAnim(defaultAname.data(), playOnce);

	AnimFrame* curAnimation = AssetManager::assetManager->animationDatas[getCurAnim()];
	if (!curAnimation) return;
	if (animation) curFrame = animation->getBoneFrame(curAnimation, time, animEnd);
	if(!animEnd) time += velocity * 0.0004;
	else if (animEnd && loop) time = 0.0;
	else if (animEnd && !loop && !playOnce && !moving) {
		setCurAnim(defaultAname.data(), playOnce);
		time = 0.0;
	}
}
