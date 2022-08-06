#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

// 最近与最远位置
float near = 0.1; 
float far  = 100.0; 

// 坐标-->>>深度
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // NDC坐标
    // (0~1)-->(-1~1)

    // F(depth)=( 1/z - 1/near ) / (  1/far - 1/near )
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{    
    // FragColor = texture(texture1, TexCoords);
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1.0);
    // FragColor = vec4(vec3(gl_FragCoord.z),1.0);
}