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
	VBOmanager()
	{
		num = 1;
		vaoLength = 0;
	}
	void addVAO(unsigned int* vao)
	{
		glGenVertexArrays(1, &*vao);

		//新建VAO对象
		_vao = new unsigned int*;
		_vao[vaoLength] = vao;

		vaoLength++;
	}
	void addStaticBuffer(unsigned int vbo, float *data={}, int size = 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		if (data){
			//std::cout << "sizeof:" << size << ",data:" << data << endl;
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		}
		else {
			//std::cout << "No VBO BUFFER DATA:" << endl;
		}		
	}


	void BindVAO(unsigned int vao)
	{
		glBindVertexArray(vao);
	}

	void addVertex(int maxLength,vector<int> alloc)
	{
		//glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
		int sum = 0;
		//std::cout <<"alloc.size():" << alloc.size() << std::endl;

		for (int i = 0; i < alloc.size(); i++)
		{
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, maxLength * sizeof(float), (void*)(sum));
			sum += alloc[i];
			//std::cout <<"sum:" << sum << "maxLength:" << maxLength << endl;
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