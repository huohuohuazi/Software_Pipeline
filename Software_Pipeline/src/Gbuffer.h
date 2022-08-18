#ifndef GBUFFER_H
#define GBUFFER_H

#include <glad/glad.h>//Ҫ��������ͷ�ļ�֮ǰ


class GBuffer
{
public:
	unsigned int gBuffer;// FrameBuffer
	unsigned int PositionDepthCBO, NormalCBO, ColorAndSpecularCBO;// Color Buffer
	unsigned int RBO;// RenderBuffer = Depth (+ Stencil)
	// ����RBO��FBO��RBOͳһ

	unsigned int viewPosition, viewNormal;

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
		SSAOBuffer();

		//unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		//glDrawBuffers(3, attachments);

		unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 , GL_COLOR_ATTACHMENT4};
		glDrawBuffers(5, attachments);

		RenderBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


private:
	// λ�û���CBO
	void PositionBuffer()
	{
		glGenTextures(1, &PositionDepthCBO);
		glBindTexture(GL_TEXTURE_2D, PositionDepthCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// GL_CLAMP_TO_EDGE�������װ��������֤�����ǲ��᲻С�Ĳ���������Ļ�ռ�������Ĭ����������֮������ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PositionDepthCBO, 0);
	}
	// ���߻���CBO
	void NormalBuffer()
	{
		glGenTextures(1, &NormalCBO);
		glBindTexture(GL_TEXTURE_2D, NormalCBO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalCBO, 0);
	}
	// ��/��������ɫ����CBO
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
	// SSAO����Ӱ����CBO
	void SSAOBuffer()
	{
		//glGenTextures(1, &ssaoCBO);
		//glBindTexture(GL_TEXTURE_2D, ssaoCBO);
		//// ��һ���Ҷ�ͼ(������DepthComponent����Ȼ��¼�ľ��������Ϣ��)
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, ssaoCBO, 0);


		glGenTextures(1, &viewPosition);
		glBindTexture(GL_TEXTURE_2D, viewPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// GL_CLAMP_TO_EDGE�������װ��������֤�����ǲ��᲻С�Ĳ���������Ļ�ռ�������Ĭ����������֮������ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, viewPosition, 0);


		glGenTextures(1, &viewNormal);
		glBindTexture(GL_TEXTURE_2D, viewNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, viewNormal, 0);


	}

	// ��Ȼ���RBO
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