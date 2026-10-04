#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location=0)in vec3 vertexPosition_modelspace;
layout(location=1)in vec3 normal_modelspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPosition;
out vec3 normal;


void main(){
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position=projection*view*model*vec4(vertexPosition_modelspace,1);
	fragPosition=vec3(model*vec4(vertexPosition_modelspace,1.f));
	normal= mat3(transpose(inverse(model))) * normal_modelspace;
	// normal_modelspace;
}

