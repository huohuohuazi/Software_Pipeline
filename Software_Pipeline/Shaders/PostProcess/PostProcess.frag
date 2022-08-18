#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float exposure;
uniform bool hdr;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

uniform bool On_GammaCorrection;// 是否开启Gamma矫正
uniform bool On_Bloom;// 是否开启Bloom泛光

vec3 Conv(float kernel[9]);
//vec4 Grayscale(vec4 color_in);
//vec3 EdgeDetect();
//vec3 Blur();

void main()
{ 
    // 获取当前坐标：
    // if(gl_FragCoord.x<720)
    // 可以用于技术演示等，将画面分成两部分，同样可以用于其他着色器的测试

    FragColor = texture(screenTexture, TexCoords);
    if(On_Bloom)
    {
        vec3 bloomcolor = texture(bloomTexture, TexCoords).rgb;
        FragColor.rgb += bloomcolor;
    }
    

    // 考虑HDR
    FragColor.rgb = vec3(1.0) - exp(-FragColor.rgb * exposure);

    // Gamma矫正
    if(On_GammaCorrection)
    {
        float gamma = 2.2;
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    }
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
    // 边缘采样可能会出现因为循环边采样而产生的条纹，此时可以设置纹理采样为
    // GL_CLAMP_TO_EDGE(重复采样边缘)

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    vec3 sampleTex[9];

    for(int i = 0; i < 9; i++)
    {
        //FragColor = texture(screenTexture, TexCoords);
        // Texcoords是uv坐标，进行对偏移的亚像素重新采样
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 result = vec3(0.0);
    for(int i = 0; i < 9; i++)
        result += sampleTex[i] * kernel[i];

    return result;
}

// 边缘检测
vec3 EdgeDetect()
{
     // 卷积核计算
    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
        );
    vec3 result=Conv(kernel);
    return result;

}

