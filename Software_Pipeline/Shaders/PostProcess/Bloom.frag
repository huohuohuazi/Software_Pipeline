#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D lightTexture;

// 灯光值输出到指定的帧缓冲对象上
uniform bool horizontal;

// 对于8*8的卷积核
// 左侧/右侧（含自己）五个像素的权值
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 offset = 1.0 / textureSize(lightTexture, 0);
    vec3 result = texture(lightTexture, TexCoords).rgb * weight[0];
    // 水平方向
    // 每次采样共8个像素
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(lightTexture, TexCoords + vec2(offset.x * i, 0.0)).rgb * weight[i];
            result += texture(lightTexture, TexCoords - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    // 垂直方向
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(lightTexture, TexCoords + vec2(0.0, offset.y * i)).rgb * weight[i];
            result += texture(lightTexture, TexCoords - vec2(0.0, offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}