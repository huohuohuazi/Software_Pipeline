#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform samplerCube depthMap;
uniform float lightPos;
uniform float far_plane;
uniform float near_plane;
uniform vec3 center;


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}
void main()
{             
    vec3 fragToCenter = FragPos - center; 
    float depthValue = texture(depthMap, fragToCenter).r;
   // FragColor = vec4(vec3(depthValue), 1.0);
    //FragColor = vec4(vec3(LinearizeDepth(depthValue)/far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue/far_plane), 1.0); // orthographic
    //FragColor=vec4(vec3(0.2), 1.0);
}