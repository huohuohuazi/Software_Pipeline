#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLM/glm.hpp>
#include "VBO.h"
#include "Shader.h"

struct Object{
	unsigned int VAO;

	glm::mat4 model;
	
};

class Scene
{
public:
	Object* objects;
};





#endif