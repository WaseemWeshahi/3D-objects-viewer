#version 150
in vec3 vColor;
in vec2 normal;
in vec4 vpos;
in vec2 texCoord;
out vec4 fColor;

uniform sampler2D texture;
uniform float u_light_ambient[100];
uniform float u_light_spec[100];
uniform float u_light_diff[100];
uniform int material_shiness;
uniform float u_light_position[100];
uniform vec4 u_matirial_diffuse;
uniform int phong_shading;
uniform vec4 u_matirial_specular;
uniform int first_time;
uniform int number_of_lights;
uniform int a_flat;
uniform int with_bloom;
uniform int with_blur;
uniform int with_tex;
vec3 CalcPointLight(vec3 norm, vec3 viewDir, vec4 light_position,vec4 light_ambient,vec4 light_diff,vec4 light_spec);
void main() 
{ 
 
  if(with_tex==1)
  {
	fColor = textureLod(texture,texCoord,0);
  }
  else{
	if( number_of_lights !=0)
	{
		if(a_flat ==1)
		{
			int i;
			vec4 temp1;

			temp1.x = u_light_position[0];
			temp1.y = u_light_position[0+1];
			temp1.z = u_light_position[0+2];
			temp1.w = u_light_position[0+3];

			vec3 l_pos = vec3(temp1);
			vec3 lightDirection = l_pos - vec3(vpos);
			float cos_angle;

			vec3 vertex_normal = vec3(normal,0.0);
			vec3 norm = normalize(vertex_normal);

			lightDirection = normalize(lightDirection);

			if(dot(norm, lightDirection) < 0)
			{
				cos_angle =0;
			}
			else
			{
				cos_angle = dot(norm, lightDirection);
			}

			vec3 new_color = vec3(vColor * cos_angle); // maybe the sum? also check the calc color
			
			for (i = 4; i < 4*(number_of_lights-1); i++)
			{
				temp1.x = u_light_position[i];
				temp1.y = u_light_position[i+1];
				temp1.z = u_light_position[i+2];
				temp1.w = u_light_position[i+3];

				vec3 pos = vec3(temp1);
				vec3 lightDirection = pos - vec3(vpos);
				float cos_angle;
				norm = normalize(norm);
				lightDirection = normalize(lightDirection);
				if(dot(norm, lightDirection) < 0)
				{
					cos_angle =0;
				}
				else
				{
					cos_angle = dot(norm, lightDirection);
				}
				new_color += vec3(vColor * cos_angle);
			}
			fColor = vec4(new_color,1.0);
			if(with_bloom == 1)
			{
				float brightness = dot(fColor.xyz, vec3(0.2126, 0.7152, 0.0722));
				if(brightness > 1.0)
				{
					
						const float gamma = 2.2;
						vec3 hdrColor = textureLod(texture,texCoord,0).xyz;      
						vec3 bloomColor = fColor.xyz;
						hdrColor += bloomColor; // additive blending
						// tone mapping
						vec3 result = vec3(1.0) - exp(-hdrColor );
						result = pow(result, vec3(1.0 / gamma));
						fColor = vec4(result, 1.0);
					
				}
				 else
				{
					fColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}
		}
		else if (phong_shading == 1)
		{
			int i;
			int j=0;

			vec3 result;
			result.x = 0.0;
			result.y = 0.0;
			result.z = 0.0;


			vec3 vertex_normal = vec3(normal,0.0);
			vec3 norm = normalize(vertex_normal);

			vec3 viewDir = normalize(-1.0*vec3( vpos));
				
			for(i=0;i<4*number_of_lights;i+=4)
			{
				
				vec4 temp1,temp2,temp3,temp4;

				temp1.x = u_light_position[i];
				temp1.y = u_light_position[i+1];
				temp1.z = u_light_position[i+2];
				temp1.w = u_light_position[i+3];

				temp2.x = u_light_ambient[i];
				temp2.y = u_light_ambient[i+1];
				temp2.z = u_light_ambient[i+2];
				temp2.w = u_light_ambient[i+3];

				temp3.x = u_light_diff[i];
				temp3.y = u_light_diff[i+1];
				temp3.z = u_light_diff[i+2];
				temp3.w = u_light_diff[i+3];
			
				temp4.x = u_light_spec[i];
				temp4.y = u_light_spec[i+1];
				temp4.z = u_light_spec[i+2];
				temp4.w = u_light_spec[i+3];

				result = result + CalcPointLight(norm,viewDir,temp1,temp2, temp3,temp4);
			
			
			}
			fColor = vec4(result,1.0);
			if(with_bloom == 1)
			{
				float brightness = dot(fColor.xyz, vec3(0.2126, 0.7152, 0.0722));
				if(brightness > 1.0)
				{
					
						const float gamma = 2.2;
						vec3 hdrColor = textureLod(texture,texCoord,0).xyz;      
						vec3 bloomColor = fColor.xyz;
						hdrColor += bloomColor; // additive blending
						// tone mapping
						vec3 result = vec3(1.0) - exp(-hdrColor  );
						result = pow(result, vec3(1.0 / gamma));
						fColor = vec4(result, 1.0);
					
				}
					
			}
		}
		else
		{
			fColor = vec4(vColor,vColor.x);
			if(with_bloom == 1)
			{
				float brightness = dot(fColor.xyz, vec3(0.2126, 0.7152, 0.0722));
				if(brightness > 1.0)
				{
					
						const float gamma = 2.2;
						vec3 hdrColor = textureLod(texture,texCoord,0).xyz;      
						vec3 bloomColor = fColor.xyz;
						hdrColor += bloomColor; // additive blending
						// tone mapping
						vec3 result = vec3(1.0) - exp(-hdrColor  );
						result = pow(result, vec3(1.0 / gamma));
						fColor = vec4(result, 1.0);
					
				
				}
					
			}
		}
	}	
	else
	{
		if(first_time==1)
		{
			fColor = textureLod(texture,texCoord,0);
			if(with_bloom == 1)
			{
				float brightness = dot(fColor.xyz, vec3(0.2126, 0.7152, 0.0722));
				if(brightness > 1.0)
				{
					
						const float gamma = 2.2;
						vec3 hdrColor = textureLod(texture,texCoord,0).xyz;      
						vec3 bloomColor = fColor.xyz;
						hdrColor += bloomColor; // additive blending
						// tone mapping
						vec3 result = vec3(1.0) - exp(-hdrColor  );
						result = pow(result, vec3(1.0 / gamma));
									
				}
					
			}
		}
		else
		{
			fColor = vec4(vColor,vColor.x);
			if(with_bloom == 1)
			{
				float brightness = dot(fColor.xyz, vec3(0.2126, 0.7152, 0.0722));
				if(brightness > 1.0)
				{
					
						const float gamma = 2.2;
						vec3 hdrColor = textureLod(texture,texCoord,0).xyz;      
						vec3 bloomColor = fColor.xyz;
						hdrColor += bloomColor; // additive blending
						// tone mapping
						vec3 result = vec3(1.0) - exp(-hdrColor  );
						result = pow(result, vec3(1.0 / gamma));
						fColor = vec4(result, 1.0);
					
				}
					
			}
		}
	}
	}
}

vec3 CalcPointLight(vec3 norm, vec3 viewDir, vec4 light_position,vec4 light_ambient,vec4 light_diff,vec4 light_spec)
{
	int i;
	
	vec3 result;
	
	vec3 ambient = vec3(light_ambient) * vColor;
  	
	// diffuse 
    vec3 lightDir = normalize(vec3(light_position - vpos));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(light_diff) * diff *(vec3( u_matirial_diffuse));
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(clamp(dot(viewDir, reflectDir),0.0, 1.0), 0.0), material_shiness);
    vec3 specular = spec * vec3(light_spec) * vec3( u_matirial_specular);  
    

	result = specular + ambient + diffuse; 	
	return result;
}
