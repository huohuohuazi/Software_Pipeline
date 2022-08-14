#ifndef GBUFFER_H
#define GBUFFER_H

#include <glad/glad.h>//要放在所有头文件之前


class GBuffer
{
public:
	unsigned int gBuffer;// FrameBuffer
	unsigned int PositionCBO, NormalCBO, ColorAndSpecularCBO;// Color Buffer
	unsigned int RBO;// RenderBuffer = Depth (+ Stencil)


public:
	GBuffer()
	{
		// GBuffer FBO
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// 位置、法线、折/反射、颜色信息
		// 一个位置向量、一个法向量，一个颜色向量，一个镜面强度值，是渲染一个光照模型的必要参数
		PositionBuffer();
		NormalBuffer();
		ColorAndSpecularBuffer();

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		RenderBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	// 位置缓冲
	void PositionBuffer()
	{
		glGenTextures(1, &PositionCBO);
		glBindTexture(GL_TEXTURE_2D, PositionCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PositionCBO, 0);
	}
	// 法线缓冲
	void NormalBuffer()
	{
		glGenTextures(1, &NormalCBO);
		glBindTexture(GL_TEXTURE_2D, NormalCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalCBO, 0);
	}
	// 折/反射与颜色缓冲
	void ColorAndSpecularBuffer()
	{
		glGenTextures(1, &ColorAndSpecularCBO);
		glBindTexture(GL_TEXTURE_2D, ColorAndSpecularCBO);
		// 折/反射与颜色两种信息整合到RGBA里面
		// RGB是颜色信息，A通道是镜面强度信息
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, ColorAndSpecularCBO, 0);
	}
	// 深度缓冲
	void RenderBuffer()
	{
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Width);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

		// 和FBO统一
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

};

#endif