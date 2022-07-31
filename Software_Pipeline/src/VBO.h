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
	// 重载构造函数
	VBOmanager(unsigned int* vbo)
	{
		num = 1;
		vaoLength = 0;
		glGenBuffers(1, &*vbo);
		_vbo = vbo;
	}
	// 默认构造函数
	VBOmanager()
	{
		num = 1;
		vaoLength = 0;
	}
	// 为状态机添加一个VAO对象
	void addVAO(unsigned int* vao)
	{
		glGenVertexArrays(1, &*vao);

		//新建VAO对象
		_vao = new unsigned int*;
		_vao[vaoLength] = vao;

		vaoLength++;
	}

	// 为VBO填充数据
	void addStaticBuffer(unsigned int vbo, float *data={}, int size = 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		if (data){
			std::cout << "sizeofData=" << size << endl;
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		}	

	}

	// 开始输入VAO数据
	void BindVAO(unsigned int vao)
	{
		glBindVertexArray(vao);
	}

	// 接着BindVAO，把顶点数据添加至VAO中
	void addVertex(int maxLength,vector<int> alloc)
	{
		//glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
		int sum = 0;

		for (int i = 0; i < alloc.size(); i++)
		{
			// position attribute
			//std::cout << "i=" << i << " ,sum=" << sum << " ,maxLength=" << maxLength << endl;
			std::cout << i << " " << alloc[i] << " " << maxLength << " " << sum << endl;
			glVertexAttribPointer(i, alloc[i], GL_FLOAT, GL_FALSE, maxLength * sizeof(float), (void*)(sum * sizeof(float)));
			sum += alloc[i];
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