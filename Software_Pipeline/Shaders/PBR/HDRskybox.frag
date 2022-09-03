#version 430 core
out vec4 FragColor;

in vec3 Position;

// environment cubeMap
uniform samplerCube environmentMap;

void main()
{
    //vec3 envColor = texture(environmentMap, Position).rgb;
    vec3 envColor = textureLod(environmentMap, Position,1.2).rgb;// Sample the mipmap
    // In Skybox.frag : FragColor = texture(skybox, TexCoords);


    // gamacorrection
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); 

    FragColor = vec4(envColor, 1.0);
    // FragColor=vec4(vec3(1.0),1.0);
}