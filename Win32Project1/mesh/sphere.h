/*
 * sphere.h
 *
 *  Created on: 2017-4-2
 *      Author: a
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "mesh.h"

class Sphere: public Mesh {
private:
	int longitude,latitude;
	vec3 caculateTangent(int j, int i);
public:
	Sphere(int m,int n);
	Sphere(const Sphere& rhs);
	virtual ~Sphere();
	virtual void initFaces();
};


#endif /* SPHERE_H_ */
