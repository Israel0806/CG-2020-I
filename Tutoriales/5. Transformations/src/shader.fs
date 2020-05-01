#version 330 core
out vec4 FragColor;  
  
uniform vec4 changingColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixer;
out vec2 TexCoordFlip;

void main()
{
    TexCoordFlip.x = 1.0 - TexCoord.x;
    TexCoordFlip.y = TexCoord.y;
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoordFlip), mixer);
}