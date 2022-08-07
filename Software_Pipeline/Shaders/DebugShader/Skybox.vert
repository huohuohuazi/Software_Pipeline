#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

// 没有M矩阵
uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // 因为天空盒的大小只是1*1*1，导致大多数物体都在它后面，因此需要让计算后的深度值恒等于1
    // 透视除法实在片段着色器后执行的，(x,y,z /= w)，把z设置成w可以让z分量恒为1
    gl_Position = pos.xyww;
}