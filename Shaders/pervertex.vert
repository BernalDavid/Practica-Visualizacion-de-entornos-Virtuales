#version 120

uniform mat4 modelToCameraMatrix;	//modelView
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 modelToClipMatrix;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient;  // rgb

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

uniform struct material_t {
	vec3  diffuse;
	vec3  specular;
	float alpha;
	float shininess;
} theMaterial;

attribute vec3 v_position; // Model space
attribute vec3 v_normal;   // Model space
attribute vec2 v_texCoord;	//coordenadas de textura

varying vec4 f_color;
varying vec2 f_texCoord;

//variables atributo: geometria del vertice
//variables varying: de vertex shader a fragment shader (relacionadas con la escena)


float lambert_factor(vec3 n, vec3 l) {
	//SI EL PRODUCTO ESCALAR ES NEGATIVO, SE DEVUELVE 0
	return max( dot(n,l), 0.0); //devolver producto escalar de n *l
}

void aporte_direccional(in int i, in vec3 l, in vec3 n, inout vec3 acumulador) {

	//NoL lo obtenemos con LAMBERT
	float NoL = lambert_factor(n,l); // aporte de Lambert 

	if (NoL > 0.0) {
		acumulador += NoL * theMaterial.diffuse * theLights[i].diffuse;
	}
}

void aporte_posicional(in int i, in vec3 l, in vec3 n, inout vec3 acumulador) {
	
	//LAMBERT
	float NoL = lambert_factor(n,l);
	
	if (NoL > 0.0) {
		acumulador += NoL * theLights[i].diffuse;
	}
}


/* NOTAS

CLASE 14-03
calculo de color
COLOR_DIFUSO_DEL_MATERIAL * COLOR_DIFUSO_DE_LA_LUZ(i) * IRRADIANCIA_DE_LA_LUZ(i)


theLights[i].position.x es la posicion x de la luz i-esima (x,y,z,w)
f_color.rgb para acceder a los tres componentes de color

*/

void main() {
	vec3 L, N, V;
	vec3 positionEye;

	vec3 acumulador_difuso = vec3(0.0, 0.0, 0.0);


	//positionEye = posicion del vertice en el sistema de coordenadas de la camara
	vec4 positionEye4 = modelToCameraMatrix * vec4(v_position, 1.0); //posicion en coordenadas homogeneas

	//obtener x y z de positionEye4
	positionEye = positionEye4.xyz;


	//N = Normal del vertice en el espacio de la camara 
	vec4 N4 = modelToCameraMatrix * vec4(v_normal, 0.0);	//v_normal es vec3 y necesitamos vec4

	//normalizar el vector. Nos quedamos con las coordenadas x y z de N4
	N = normalize(N4.xyz); //ya lo tenemos en el sistema de coordenadas de la camara


	//V = vector que va del vertice a la camara
	vec4 V4 = (0.0, 0.0, 0.0, 1) - positionEye4;

	V = normalize(V4.xyz);

	//for para las luces
	for(int i=0; i < active_lights_n; ++i) {
		
		//CASO LUZ DIRECCIONAL
		if (theLights[i].position.w == 0.0) {
			//vector de la luz direccional
			L = normalize(-1.0*theLights[i].position.xyz);	//funcion normalizar de GLSL

			//CALCULAR LAMBERT (se hace detro de aporte_direccional)
			aporte_direccional(i, L, N, acumulador_difuso);
		}
		//CASO DE LA LUZ POSICIONAL
		else {
			//vector del vertice a la luz (del punto de luz - posicion camara)
			L = normalize(theLights[i].position.xyz - positionEye);

			aporte_posicional(i, L, N, acumulador_difuso);
		}
		//CASO DE LA SPOT
	}

	f_color = vec4(0.0, 0.0, 0.0, 1.0); //rojo, verde, azul, opacidad
	f_color.rgb = scene_ambient + acumulador_difuso;

	//coordenadas de textura que se le pasan del vertex-shader al fragment shader
	f_texCoord = v_texCoord;

	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
}
