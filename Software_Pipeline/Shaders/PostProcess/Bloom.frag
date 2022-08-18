#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D lightTexture;

// �ƹ�ֵ�����ָ����֡���������
uniform bool horizontal;

// ����8*8�ľ�����
// ���/�Ҳࣨ���Լ���������ص�Ȩֵ
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 offset = 1.0 / textureSize(lightTexture, 0);
    vec3 result = texture(lightTexture, TexCoords).rgb * weight[0];
    // ˮƽ����
    // ÿ�β�����8������
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(lightTexture, TexCoords + vec2(offset.x * i, 0.0)).rgb * weight[i];
            result += texture(lightTexture, TexCoords - vec2(offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    // ��ֱ����
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