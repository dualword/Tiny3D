/*
 * shadermanager.h
 *
 *  Created on: 2017-4-7
 *      Author: a
 */

#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include "shader.h"
#include <vector>

class ShaderManager {
private:
	std::map<std::string,Shader*> shaders;
	std::vector<Shader*> shaderBindTex;
public:
	ShaderManager();
	~ShaderManager();
	Shader* addShader(const char* name, const char* vs, const char* fs, const char* tc = NULL, const char* te = NULL, const char* gs = NULL);
	Shader* addShader(const char* name, const char* cs);
	Shader* findShader(const char* name);
	void unbindAllTex();
	void compile();
	void addShaderBindTex(Shader* shader);
	std::vector<Shader*>* getShaderBindTex() { return &shaderBindTex; }
};


#endif /* SHADERMANAGER_H_ */
