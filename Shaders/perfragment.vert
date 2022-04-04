#version 120

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_texCoord;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 modelToClipMatrix;

varying vec3 f_position;      // camera space
varying vec3 f_viewDirection; // camera space
varying vec3 f_normal;        // camera space
varying vec2 f_texCoord;

//f_position en fragment shader: posicion del pixel en el sistema de coordenadas de la camara

/* 
	En vertex shader se guarda toda la informacion para vertices
	AL pasarlo a fragment shader, toda esa informacion hace referencia al pixel
	A traves de las variables varying se tranfiere la informacion entre ambos "mundos"
*/

void main() {
	
	//no se normalizan porque los pasamos al fragment shader
	//se normalizara en fragment shader
	
	//se calcula igual que en pervertex.vert
	vec4 f_position4 = (modelToCameraMatrix * vec4(v_position, 1.0));
	f_position = f_position4.xyz;

	vec4 f_viewDirection4 = (0,0,0,1) - f_position4;
	f_viewDirection = f_viewDirection4.xyz;

	vec4 f_normal4 = (modelToCameraMatrix * vec4(v_normal,0));
	f_normal = f_normal;
	
	
	//f_texCoord = v_texCoord;

	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
}



