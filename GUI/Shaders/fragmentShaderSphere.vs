#version 330 core

in vec3 fragPosition;
in vec3 normal;

// Ouput data
out vec4 FragColor;

uniform vec3 lightPos;

// vec3(1,1,0.1);

void main(){
  
  vec3 lightColor=vec3(1.,1.,1.);
  vec3 objectColor=vec3(0.2,0.2,0.2);
  
  float ambientStrength=0.9;
  vec3 ambient=ambientStrength*lightColor;
  
  vec3 norm =normalize(normal);
  vec3 lightDir= (lightPos-fragPosition);
  float diff=max(dot(norm,lightDir),0.0);
  vec3 diffuse=diff*lightColor;
  
  vec3 result=(ambient+diffuse)*objectColor;
  

 // FragColor=vec4(result,0.9);
 FragColor=vec4(1,1,1,1);
}