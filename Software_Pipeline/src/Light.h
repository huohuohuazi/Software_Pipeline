#ifndef DIRLIGHT_H
#define DIRLIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 一些全局变量
const int SHADOW_WIDTH = 1280;
const int SHADOW_HEIGHT = 720;

const float LightYAW = -90.0f;
const float LightPITCH = 0.0f;

const float LightSPEED = 2.5f;
const float LightSENSITIVITY = 0.1f;
const float LightZOOM = 45.0f;

#include <vector>

class DirLight
{
public:
	glm::vec3 Position;//初始位置
	glm::vec3 Front;//正向向量
	glm::vec3 Up;//vup

	glm::vec3 Right;//右向量
	glm::vec3 WorldUp;//世界z正轴

	// 旋转角
	float Yaw;
	float Pitch;

	// 移动速度 
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	float lastX = SHADOW_WIDTH / 2, lastY = SHADOW_HEIGHT / 2;

	// 默认构造函数
	DirLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = LightYAW, float pitch = LightPITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(LightSPEED), MouseSensitivity(LightSENSITIVITY), Zoom(LightZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateLightCameraVectors();
	}

	glm::mat4 GetView()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}
private:
	void updateLightCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

};






#endif