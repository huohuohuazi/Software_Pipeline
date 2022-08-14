#ifndef GBUFFER_H
#define GBUFFER_H

#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ


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

		// λ�á����ߡ���/���䡢��ɫ��Ϣ
		// һ��λ��������һ����������һ����ɫ������һ������ǿ��ֵ������Ⱦһ������ģ�͵ı�Ҫ����
		PositionBuffer();
		NormalBuffer();
		ColorAndSpecularBuffer();

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		RenderBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	// λ�û���
	void PositionBuffer()
	{
		glGenTextures(1, &PositionCBO);
		glBindTexture(GL_TEXTURE_2D, PositionCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PositionCBO, 0);
	}
	// ���߻���
	void NormalBuffer()
	{
		glGenTextures(1, &NormalCBO);
		glBindTexture(GL_TEXTURE_2D, NormalCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalCBO, 0);
	}
	// ��/��������ɫ����
	void ColorAndSpecularBuffer()
	{
		glGenTextures(1, &ColorAndSpecularCBO);
		glBindTexture(GL_TEXTURE_2D, ColorAndSpecularCBO);
		// ��/��������ɫ������Ϣ���ϵ�RGBA����
		// RGB����ɫ��Ϣ��Aͨ���Ǿ���ǿ����Ϣ
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, ColorAndSpecularCBO, 0);
	}
	// ��Ȼ���
	void RenderBuffer()
	{
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Width);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

		// ��FBOͳһ
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

		// finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

};

#endif