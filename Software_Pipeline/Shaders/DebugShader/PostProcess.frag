#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

//vec3 Conv(float kernel[9]);
//vec4 Grayscale(vec4 color_in);
//vec3 EdgeDetect();
//vec3 Blur();

void main()
{ 

    FragColor = texture(screenTexture, TexCoords);
    
    // FragColor=vec4(Blur(),1.0);
}

vec4 Grayscale(vec4 color_in)
{
    float average = 0.2126 * color_in.r + 0.7152 * color_in.g + 0.0722 * color_in.b;
    vec4 color_out=vec4(average, average, average, 1.0);
    return color_out;
}

vec3 Blur()
{

    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    vec3 result=Conv(kernel);
    return result;
}


vec3 Conv(float kernel[9])
{
    const float offset = 1.0 / 300.0;

     vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    vec3 sampleTex[9];

    for(int i = 0; i < 9; i++)
    {
        //FragColor = texture(screenTexture, TexCoords);
        // Texcoords��uv���꣬���ж�ƫ�Ƶ����������²���
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 result = vec3(0.0);
    for(int i = 0; i < 9; i++)
        result += sampleTex[i] * kernel[i];

    return result;
}

// ��Ե���
vec3 EdgeDetect()
{
     // ����˼���
    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
        );
    vec3 result=Conv(kernel);
    return result;

}