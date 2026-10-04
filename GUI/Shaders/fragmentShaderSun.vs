#version 330 core

in vec3 fragPosition;
in vec3 normal;

// Ouput data
out vec4 FragColor;
uniform vec3 lightPos;

// vec3(1,1,0.1);

void main(){
  
  vec3 lightColor=vec3(1.,1.,1.);
  vec3 objectColor=vec3(1,1,0);
  
  float ambientStrength=1.0f;
  vec3 ambient=ambientStrength*lightColor;
  
  // vec3 norm = normalize(normal);
  vec3 lightDir=-lightPos;//normalize(lightPos - fragPosition);
  float diff= 0.8;//max(dot(normal,lightDir),0.);
  vec3 diffuse=diff*lightColor;
  
  vec3 result=(ambient+diffuse)*objectColor;
  
  // Output color = color of the texture at the specified UV
  // FragColor=vec4(1, 1.0, 1.0, 0.8);
  
  FragColor=vec4(result,1);
}