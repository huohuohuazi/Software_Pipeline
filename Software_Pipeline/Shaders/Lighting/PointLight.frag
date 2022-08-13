#version 330 core

// 以前写作 out vec4 FragColor;
layout (location = 0) out vec4 FragColor;// --》Color_Attachment0
layout (location = 1) out vec4 BrightColor;// --》Color_Attachment1
// 现指定输出到不同的帧缓冲对象上

void main()
{
    float lighting=5.0;
    FragColor = vec4(vec3(lighting),1.0); // set alle 4 vector values to 1.0

    // 后面那个向是视角偏差？
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
    //if(FragColor.r >= 1.0)
    {
        //FragColor.rgb=vec3(0.0,1.1,0.0);
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
}