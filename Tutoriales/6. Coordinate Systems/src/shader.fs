#version 330 core

out vec4 FragColor;  
in vec2 TexCoord;  

uniform vec3 color;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform int selected;

void main()
{
//    FragColor = vec4(color,1.0f);
    if(selected == 0)
        FragColor = texture(texture1, TexCoord);
    else if(selected == 1)
        FragColor = texture(texture2, TexCoord);
    else if(selected == 2)
        FragColor = texture(texture3, TexCoord);
}