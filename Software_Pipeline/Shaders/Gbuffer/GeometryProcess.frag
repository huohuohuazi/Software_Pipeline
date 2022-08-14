#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;// 注意这里要和CBO的RGB/RGBA统一

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} frag_in;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

void main()
{    
    // 输出至指定的颜色缓冲中
    gPosition = frag_in.FragPos;// 这里是世界空间坐标
    gNormal = normalize(frag_in.Normal);
    gAlbedoSpec = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    // 存储镜面强度到A通道中
    // gAlbedoSpec.a = texture(texture_specular1, frag_in. TexCoords).r;
}  