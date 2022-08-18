#version 430 core
in vec2 TexCoords;
out float fragColor;

uniform sampler2D gSSAO;

void main() {

    vec2 texelSize = 1.0 / vec2(textureSize(gSSAO, 0));
    float result = 0.0;
    // ��ֵ�˲� --> ģ����ɫ
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(gSSAO, TexCoords + offset).r;
        }
    }
    fragColor = result / (4.0 * 4.0);
}