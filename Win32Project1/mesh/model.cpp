#include "model.h"
#include "../constants/constants.h"
#include "../material/materialManager.h"
#include "../util/util.h"
#include <stdlib.h>
#include <string.h>

Model::Model(const char* obj, const char* mtl, int vt) :Mesh() {
	vertexCount = 0;
	indexCount = 0;
	vertices = NULL;
	normals = NULL;
	tangents = NULL;
	texcoords = NULL;
	materialids = NULL;
	indices = NULL;
	mats.clear();
	loadModel(obj, mtl, vt);
	caculateExData();
}

Model::Model(const Model& rhs) :Mesh(rhs) {
	if (rhs.vertexCount > 0) {
		vertexCount = rhs.vertexCount;
		vertices = new vec4[vertexCount];
		normals = new vec3[vertexCount];
		tangents = new vec3[vertexCount];
		texcoords = new vec2[vertexCount];
		materialids = new int[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			vertices[i] = rhs.vertices[i];
			normals[i] = rhs.normals[i];
			tangents[i] = rhs.tangents[i];
			texcoords[i] = rhs.texcoords[i];
			materialids[i] = rhs.materialids[i];
		}
	}

	if (rhs.indexCount > 0) {
		indexCount = rhs.indexCount;
		indices = (int*)malloc(indexCount * sizeof(int));
		memcpy(indices, rhs.indices, indexCount * sizeof(int));
	}

	for (int i = 0; i < rhs.mats.size(); ++i)
		mats.push_back(rhs.mats[i]);

	caculateExData();
}

Model::~Model() {
	mats.clear();
}

void Model::loadModel(const char* obj,const char* mtl,int vt) {
	loader = new ObjLoader(obj,mtl,vt);
	initFaces();
	correctVertices(obj);
	delete loader;
}

void Model::correctVertices(const char* obj) {
	const float minVal = std::numeric_limits<float>::min();
	const float maxVal = std::numeric_limits<float>::max();
	float sx = maxVal, sy = maxVal, sz = maxVal;
	float lx = minVal, ly = minVal, lz = minVal;

	if (normalFaces.size() > 0) {
		for (uint n = 0; n < normalFaces.size(); ++n) {
			FaceBuf* buf = normalFaces[n];
			for (int i = 0; i < buf->count; ++i) {
				int index = indices[buf->start + i];
				vec4 vertex = vertices[index];
				sx = sx > vertex.x ? vertex.x : sx;
				sy = sy > vertex.y ? vertex.y : sy;
				sz = sz > vertex.z ? vertex.z : sz;
				lx = lx < vertex.x ? vertex.x : lx;
				ly = ly < vertex.y ? vertex.y : ly;
				lz = lz < vertex.z ? vertex.z : lz;
			}
		}
	} else {
		for (int i = 0; i < vertexCount; ++i) {
			vec4 vertex = vertices[i];
			sx = sx > vertex.x ? vertex.x : sx;
			sy = sy > vertex.y ? vertex.y : sy;
			sz = sz > vertex.z ? vertex.z : sz;
			lx = lx < vertex.x ? vertex.x : lx;
			ly = ly < vertex.y ? vertex.y : ly;
			lz = lz < vertex.z ? vertex.z : lz;
		}
	}

	vec3 halfSize = vec3(lx - sx, ly - sy, lz - sz) * 0.5;
	vec3 origin = vec3(sx, sy, sz) + halfSize;
	vec4 offset = vec4(origin, 0.0);
	for (int i = 0; i < vertexCount; ++i) 
		vertices[i] -= offset;
}

void Model::initFaces() {
	vertexCount = loader->vCount;
	indexCount = loader->faceCount * 3;
	vertices = new vec4[indexCount];
	for(int i=0;i<vertexCount;i++) {
		vertices[i].x=loader->vArr[i][0];
		vertices[i].y=loader->vArr[i][1];
		vertices[i].z=loader->vArr[i][2];
		vertices[i].w=1.0;
	}

	normals = new vec3[indexCount];
	tangents = new vec3[indexCount];
	texcoords = new vec2[indexCount];
	materialids = new int[indexCount];
	indices = (int*)malloc(indexCount*sizeof(int));

	std::vector<int> statlst; statlst.clear();
	std::vector<int> startlst; startlst.clear();
	std::vector<int> countlst; countlst.clear();
	int laststat = -1;

	std::map<int, bool> texcoordMap; texcoordMap.clear();

	int dupIndex = vertexCount;
	for (int i=0;i<loader->faceCount;i++) {
		int index1=loader->fvArr[i][0]-1;
		int index2=loader->fvArr[i][1]-1;
		int index3=loader->fvArr[i][2]-1;

		vec3 n1(loader->vnArr[loader->fnArr[i][0]-1][0],
				loader->vnArr[loader->fnArr[i][0]-1][1],
				loader->vnArr[loader->fnArr[i][0]-1][2]);
		vec3 n2(loader->vnArr[loader->fnArr[i][1]-1][0],
				loader->vnArr[loader->fnArr[i][1]-1][1],
				loader->vnArr[loader->fnArr[i][1]-1][2]);
		vec3 n3(loader->vnArr[loader->fnArr[i][2]-1][0],
				loader->vnArr[loader->fnArr[i][2]-1][1],
				loader->vnArr[loader->fnArr[i][2]-1][2]);

		vec2 c1(loader->vtArr[loader->ftArr[i][0]-1][0],
				loader->vtArr[loader->ftArr[i][0]-1][1]);
		vec2 c2(loader->vtArr[loader->ftArr[i][1]-1][0],
				loader->vtArr[loader->ftArr[i][1]-1][1]);
		vec2 c3(loader->vtArr[loader->ftArr[i][2]-1][0],
				loader->vtArr[loader->ftArr[i][2]-1][1]);
		
		// Duplicate vertex if texcoord not the same
		if (texcoordMap.find(index1) != texcoordMap.end() && texcoords[index1] != c1) {
			int newIndex = dupIndex++;
			vertices[newIndex] = vertices[index1];
			index1 = newIndex;
		}
		if (texcoordMap.find(index2) != texcoordMap.end() && texcoords[index2] != c2) {
			int newIndex = dupIndex++;
			vertices[newIndex] = vertices[index2];
			index2 = newIndex;
		}
		if (texcoordMap.find(index3) != texcoordMap.end() && texcoords[index3] != c3) {
			int newIndex = dupIndex++;
			vertices[newIndex] = vertices[index3];
			index3 = newIndex;
		}
		
		normals[index1] = n1; normals[index2] = n2; normals[index3] = n3;
		texcoords[index1] = c1; texcoords[index2] = c2; texcoords[index3] = c3;
		texcoordMap[index1] = true; texcoordMap[index2] = true; texcoordMap[index3] = true;

		vec3 faceTangent = CaculateTangent(vertices[index1], vertices[index2], vertices[index3], texcoords[index1], texcoords[index2], texcoords[index3]);
		tangents[index1] = faceTangent;
		tangents[index2] = faceTangent;
		tangents[index3] = faceTangent;

		indices[i * 3 + 0] = index1;
		indices[i * 3 + 1] = index2;
		indices[i * 3 + 2] = index3;

		int mid = loader->mtlLoader->objMtls[loader->mtArr[i]];
		materialids[index1] = mid;
		materialids[index2] = mid;
		materialids[index3] = mid;

		int curstat = 0;
		Material* mat = MaterialManager::materials->find(mid);
		if (mat && mat->singleFace) curstat = 1;
		else if (mat && mat->transparent) curstat = 2;
		else curstat = 0;
		if (laststat == curstat)
			countlst[countlst.size() - 1] += 3;
		else {
			startlst.push_back(i * 3);
			countlst.push_back(3);
			statlst.push_back(curstat);
		}
		laststat = curstat;
	}

	for (uint i = 0; i < statlst.size(); i++) {
		int stat = statlst[i];
		if (stat == 0) normalFaces.push_back(new FaceBuf(startlst[i], countlst[i]));
		else if (stat == 1) singleFaces.push_back(new FaceBuf(startlst[i], countlst[i]));
		else if (stat == 2) transpFaces.push_back(new FaceBuf(startlst[i], countlst[i]));
	}
	vertexCount = dupIndex;

	bool allNormal = singleFaces.size() == 0 && transpFaces.size() == 0 && normalFaces.size() > 0;
	bool allSingle = normalFaces.size() == 0 && transpFaces.size() == 0 && singleFaces.size() > 0;
	bool allTransp = normalFaces.size() == 0 && singleFaces.size() == 0 && transpFaces.size() > 0;
	bool hasFaces = normalFaces.size() > 0 || singleFaces.size() > 0 || transpFaces.size() > 0;

	if (hasFaces && !allNormal && !allSingle && !allTransp) {
		int* tmp = (int*)malloc(indexCount * sizeof(int));
		int curIndex = 0, singleStart = 0, singleCount = 0, normalStart = 0, normalCount = 0, transpStart = 0, transpCount = 0;
		singleStart = curIndex;
		for (uint i = 0; i < singleFaces.size(); i++) {
			FaceBuf* buf = singleFaces[i];
			memcpy(tmp + curIndex, indices + buf->start, buf->count * sizeof(int));
			curIndex += buf->count;
			singleCount += buf->count;
		}
		normalStart = curIndex;
		for (uint i = 0; i < normalFaces.size(); i++) {
			FaceBuf* buf = normalFaces[i];
			memcpy(tmp + curIndex, indices + buf->start, buf->count * sizeof(int));
			curIndex += buf->count;
			normalCount += buf->count;
		}
		transpStart = curIndex;
		for (uint i = 0; i < transpFaces.size(); i++) {
			FaceBuf* buf = transpFaces[i];
			memcpy(tmp + curIndex, indices + buf->start, buf->count * sizeof(int));
			curIndex += buf->count;
			transpCount += buf->count;
		}
		free(indices);
		indices = tmp;

		clearFaceBuf();
		if (singleCount > 0) singleFaces.push_back(new FaceBuf(singleStart, singleCount));
		if (normalCount > 0) normalFaces.push_back(new FaceBuf(normalStart, normalCount));
		if (transpCount > 0) transpFaces.push_back(new FaceBuf(transpStart, transpCount));
	}
}
