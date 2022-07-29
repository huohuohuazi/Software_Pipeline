#include <glad/glad.h>//要放在所有头文件之前
//#include <vector>



class VBOmanager
{
public:
	unsigned int* _vbo;

	unsigned int** _vao;
	unsigned int* _eao;
	int num;
	int vaoLength;

public:
	VBOmanager(unsigned int* vbo)
	{
		num = 1;
		vaoLength = 0;
		glGenBuffers(1, &*vbo);
		_vbo = vbo;
	}
	void addVAO(unsigned int* vao)
	{
		glGenVertexArrays(1, &*vao);
		glBindVertexArray(*vao);

		//新建VAO对象
		_vao = new unsigned int*;
		_vao[vaoLength] = vao;

		vaoLength++;
	}
	void addStaticBuffer(float* data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	}


	void addVertex(unsigned int* vao,int maxLength,vector<int> alloc)
	{
		//glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
		int sum = 0;
		std::cout <<"alloc.size():" << alloc.size() << std::endl;

		for (int i = 0; i < alloc.size(); i++)
		{
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, maxLength * sizeof(float), (void*)(sum));
			sum += alloc[i];
			std::cout <<"sum:" << sum << endl;
			glEnableVertexAttribArray(i);
		}
	}

	void clearBuffer()
	{
		for (int i = 0; i < vaoLength; i++)
		{
			
			glDeleteVertexArrays(1, &*_vao[i]);
		}
		glDeleteBuffers(1, &*_vbo);

	}
};