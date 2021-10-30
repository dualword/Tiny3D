#include "cubemap.h"
#include "../render/render.h"

CubeMap::CubeMap(const char* xpos,const char* xneg,const char* ypos,
		const char* yneg,const char* zpos,const char* zneg, bool useMip) {
	xposImg=new ImageLoader(xpos);
	xnegImg=new ImageLoader(xneg);
	yposImg=new ImageLoader(ypos);
	ynegImg=new ImageLoader(yneg);
	zposImg=new ImageLoader(zpos);
	znegImg=new ImageLoader(zneg);

	width = xposImg->width;
	height = xposImg->height;

	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_CUBE_MAP,id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB_ALPHA,
		xposImg->width, xposImg->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, xposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB_ALPHA,
		xnegImg->width, xnegImg->height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, xnegImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB_ALPHA,
		yposImg->width, yposImg->height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, yposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB_ALPHA,
		ynegImg->width, ynegImg->height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, ynegImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB_ALPHA,
		zposImg->width, zposImg->height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, zposImg->data);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB_ALPHA,
		znegImg->width, znegImg->height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, znegImg->data);

	if (useMip) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
	hnd = genBindless();
#ifndef _DEBUG 
	releaseMemory();
#endif
}

CubeMap::CubeMap(int w, int h, bool useMip, int p) {
	xposImg = NULL, xnegImg = NULL;
	yposImg = NULL, ynegImg = NULL;
	zposImg = NULL, znegImg = NULL;
	width = w, height = h;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLenum format = p > HIGH_PRE ? GL_RGBA32F : GL_SRGB_ALPHA;
	GLenum type = p > HIGH_PRE ? GL_FLOAT : GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format,
		width, height, 0, GL_RGBA, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format,
		width, height, 0, GL_RGBA, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format,
		width, height, 0, GL_RGBA, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format,
		width, height, 0, GL_RGBA, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format,
		width, height, 0, GL_RGBA, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format,
		width, height, 0, GL_RGBA, type, NULL);

	if (useMip) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	hnd = genBindless();
}

CubeMap::~CubeMap() {
	releaseMemory();
	releaseBindless(hnd);
	glDeleteTextures(1,&id);
}

void CubeMap::releaseMemory() {
	if (xposImg) delete xposImg; xposImg = NULL;
	if (xnegImg) delete xnegImg; xnegImg = NULL;
	if (yposImg) delete yposImg; yposImg = NULL;
	if (ynegImg) delete ynegImg; ynegImg = NULL;
	if (zposImg) delete zposImg; zposImg = NULL;
	if (znegImg) delete znegImg; znegImg = NULL;
}

u64 CubeMap::genBindless() {
	u64 texHnd = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(texHnd);
	return texHnd;
}

void CubeMap::releaseBindless(u64 texHnd) {
	glMakeTextureHandleNonResidentARB(texHnd);
}

