#version 330 core

//input 
// in vec3 fragPosition;
// in vec3 normalVector;
in vec3 pos;

//output
out vec4 FragColor;
uniform vec3 relativeLightPos;

void main()
{
    // // ambient
    // vec3 ambient =  ;//* material.ambient;
    // // diffuse
    // vec3 norm = normalize(normalVector);
    // vec3 lightDir = normalize(light.position - fragPosition);
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // // // specular


    
    // vec3 lightColor=vec3(1.,1.,0.);
    // vec3 objectColor=vec3(1,1,1);
    
    // float ambientStrength=1;
    // vec3 ambient=ambientStrength*lightColor;
    
    // vec3 norm = normalize(normalVector);
    // vec3 lightDir=normalize(lightPos - fragPosition);
    // float diff=max(dot(norm,lightDir),10);
    // vec3 diffuse=diff*lightColor;


    // vec3 viewDir = normalize(lightPos-pos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    // vec3 specular = lightColor * spec * 0.1f;
    // vec3 result=vec3(ambient + diffuse+specular)*objectColor;
    
    vec3 light = relativeLightPos;//normalize(lightPos);
    // if((pos.x>=0.25 && light.x>=0.25 || pos.y>=0.25 && light.y>=0.25  || pos.z>=0.25 && light.z>=0.25 ) ||
    // (pos.x<=-0.25 && light.x<=-0.25 || pos.y<=-0.25 && light.y<=-0.25  || pos.z<=-0.25 && light.z<=-0.25 ))
    // FragColor=vec4(1.,1.,0.,.5);
    // else
    // FragColor=vec4(1.,.9,0.,.1);

    float x = max(0.1,light.x);
    float y = max(0.1,light.y);
    float z = max(0.1,light.z);

    float xx = max(0.1,abs(light.x));
    float yy = max(0.1,abs(light.y));
    float zz = max(0.1,abs(light.z));

    FragColor=vec4(1.,1,0.,.1);
    
    if(pos.x>=0.25 && light.x>=0.1)
    FragColor = vec4(1,1,0,x);
    
    else if(pos.z>=0.25 && light.z>=0.1)
    FragColor = vec4(1,1,0,z);

    else if(pos.y>=0.25 && light.y>=0.1)
    FragColor = vec4(1,1,0,y);

    else if(pos.x<=-0.25 && light.x<=-0.1)
    FragColor = vec4(1,1,0,xx);

    else if(pos.y<=-0.25 && light.y<=-0.1)
    FragColor = vec4(1,1,0,yy);
    
    else if(pos.z<=-0.25 && light.z<=-0.1)
    FragColor = vec4(1,1,0,zz);
     
}