#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
//uniform bool reverse_normals;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    //vert_out.FragPos = vec3(model * vec4(position, 1.0));
    vert_out.FragPos = (model * vec4(position, 1.0)).xyz;
    vert_out.Normal = transpose(inverse(mat3(model))) * normal;
    vert_out.TexCoords = texCoords;
    // vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
}