#version 330 core
  
in vec3 Normal; 
in vec3 FragPos;  
in vec3 Color;
out vec4 FragColor;

//uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos;


void main()
{
    /// ambient light
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    /// diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    /// specular light
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}  