/*
 * terrain.h
 *
 *  Created on: 2017-7-29
 *      Author: a
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "mesh.h"
#include "chunk.h"
#include <map>

#ifndef CHUNK_SIZE
#define CHUNK_SIZE 8
#define CHUNK_INDEX_COUNT (CHUNK_SIZE * CHUNK_SIZE * 6)
#define MAP_SIZE 1028
#define	STEP_SIZE 4
#define LINE_CHUNKS ((MAP_SIZE - STEP_SIZE) / (STEP_SIZE * CHUNK_SIZE))
#endif

class Terrain: public Mesh {
private:
	unsigned char* heightMap;
	int chunkStep, stepCount;
private:
	void loadHeightMap(const char* fileName);
	float getHeight(int px,int pz);
	vec3 caculateNormal(vec3 p1,vec3 p2,vec3 p3);
	vec3 normalize(vec3 n1,vec3 n2,vec3 n3,vec3 n4,vec3 n5,vec3 n6);
	vec3 getTerrainNormal(float x,float y,float z);
	vec3 getTerrainTangent(float x, float y, float z, float u, float v, float du, float dv);
	vec3 getTangent(const vec3& normal);
	void createChunks();
	virtual void initFaces();
public:
	int blockCount;
	std::map<uint, uint*> blockIndexMap;
	uint* visualIndices;
	uint visualIndCount;
	float* visualPoints;
	uint visualPointsSize;
	std::vector<Chunk*> chunks;
public:
	Terrain(const char* fileName);
	virtual ~Terrain();
public:
	unsigned char* getHeightMap() { return heightMap; }
	void initPoint(const vec3& p, const vec3& n, const vec4& t1, const vec4& t2, uint& index);
};

#endif /* TERRAIN_H_ */
