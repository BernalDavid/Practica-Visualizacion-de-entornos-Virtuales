#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

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

uniform sampler2D texture0;

varying vec3 f_position;      // camera space
varying vec3 f_viewDirection; // camera space
varying vec3 f_normal;        // camera space
varying vec2 f_texCoord;


float lambert_factor(vec3 n, vec3 l) {
	// n es el vector normal
	// l es el vector de la luz

	//SI EL PRODUCTO ESCALAR ES NEGATIVO, SE DEVUELVE 0
	return max(dot(n,l), 0.0); //devolver producto escalar de n *l
}



float especular_factor(in vec3 n, in vec3 l, in vec3 v, in float m) {
	// n es el vector normal
	// l es el vector de la luz
	// v es el vector que va a la camara
	// m es el brillo del material (theMaterial.shininess)

	float factor_especular = 0.0;
	
	//r = 2(n*l)n -l
	vec3 r = 2*dot(n,l)*n-l;

	//se debe normalizar para realizar los calculos
	r = normalize(r);

	factor_especular = dot(r,v);

	//max(0,(r*v)^m), si r*v es positivo, calculamos el factor, si no el factor_especular = 0.0
	if (factor_especular > 0.0) {
		factor_especular = pow(factor_especular,m);
	}

	return factor_especular;

}

void aporte_direccional(in int i, in vec3 l, in vec3 n, in vec3 v, inout vec3 acumulador_difuso, inout vec3 acumulador_especular) {

	//NoL lo obtenemos con LAMBERT
	float NoL = lambert_factor(n,l); // aporte de Lambert 

	if (NoL > 0.0) {
		acumulador_difuso += NoL * theLights[i].diffuse * theMaterial.diffuse ;

		//specular_factor(n,l) * color_especular_material * color_especular_de_la_luz * factor_difuso
		
		//factor_especular = dot(n,l)* pow(factor_especular,m)  * theMaterial.specular * theLights[i].specular;
		acumulador_especular += NoL * especular_factor(n, l, v, theMaterial.shininess) * theMaterial.specular * theLights[i].specular;
	}
}

void aporte_posicional(in int i, in vec3 l, in vec3 n, in vec3 v, inout vec3 acumulador_difuso, inout vec3 acumulador_especular) {
	
	// d = distancia euclidea |pl-ps| = |L|
	float d = length(l);

	//normalizamos la distancia
	l = normalize(l);

	//LAMBERT
	float NoL = lambert_factor(n,l);
	
	if (NoL > 0.0) {
		//calcular atenuacion
		// fdist(d) = 1/ (atenuacion_constante + atenuacion_lineal*d + atenuacion_cuadratica*d^2)
		float fdist = theLights[i].attenuation[0] + theLights[i].attenuation[1]*d  + theLights[i].attenuation[2]*d*d;

		if (fdist > 0.0) {
			fdist = 1/fdist;
		} else {
			fdist = 1.0; /// No hay atenuacion ...la luz es tal como esta definida
		}

		acumulador_difuso += NoL * theLights[i].diffuse * theMaterial.diffuse * fdist;

		acumulador_especular += NoL * theLights[i].specular * especular_factor(n,l,v, theMaterial.shininess) * theMaterial.specular * fdist;

	}
}

void aporte_spotlight(in int i, in vec3 l, in vec3 n, in vec3 v, inout vec3 acumulador_difuso, inout vec3 acumulador_especular) {
	//LAMBERT
	float NoL = lambert_factor(n,l);

	float cspot = 0.0;

	if (NoL > 0.0) {
		//vector de la luz
		vec3 dir_spot = normalize(theLights[i].spotDir);
		//coseno = angulo entre los vectores  de la luz y direccional (-l = de positionEye a la posición de la luz)
		float coseno = dot(-l, dir_spot);
		//comprobar si esta dentro
		if (coseno > theLights[i].cosCutOff) {
			//cspot = cos^exponente_de_la_luz, si el coseno es positivo
			cspot = pow(coseno, theLights[i].exponent);

			acumulador_difuso += NoL * theLights[i].diffuse * theMaterial.diffuse * cspot;

			acumulador_especular += NoL * theLights[i].specular * especular_factor(n, l, v, theMaterial.shininess) *theMaterial.specular * cspot;
		}
	}	

}


void main() {

	vec3 L, N, V;

	vec3 acumulador_difuso = vec3(0.0, 0.0, 0.0);
	vec3 acumulador_especular = vec3(0.0, 0.0, 0.0);

	//f_position (posicion del pixel en el espacio de la camara)
	//f_viewDirection (vector del pixel a la camara?????)
	//f_normal (normal del pixel en el espacio de la camara)


	//N = Normal del punto en el espacio de la camara 
	N = normalize(f_normal); //f_normal ya esta en el sistema de coordenadas de la camara


	//V = vector que va del punto a la camara
	V = normalize(f_viewDirection);

	//for para las luces
	for(int i=0; i < active_lights_n; ++i) {
		
		//CASO LUZ DIRECCIONAL
		if (theLights[i].position.w == 0.0) {
			//vector de la luz direccional
			L = normalize(-1.0*theLights[i].position.xyz);	//funcion normalizar de GLSL

			//CALCULAR LAMBERT (se hace detro de aporte_direccional)
			aporte_direccional(i, L, N, V, acumulador_difuso, acumulador_especular);
		}
		//CASO DE LA LUZ POSICIONAL
		else if (theLights[i].cosCutOff == 0.0) {
			//vector del vertice a la luz (del punto de luz - posicion camara)
			L = (theLights[i].position.xyz - f_position);

			aporte_posicional(i, L, N, V, acumulador_difuso, acumulador_especular);
		}
		//CASO DE LA SPOT
		else {
			//vector del vertice a la luz (del punto de luz - posicion camara) igual que en posicional
			L = normalize(theLights[i].position.xyz - f_position);

			aporte_spotlight(i, L, N, V, acumulador_difuso, acumulador_especular);
		}
	}


	//pdf GLSL -> glFragCOlor = vec4(vColor, 1.0)
	vec4 vColor = vec4(0.0, 0.0, 0.0, 1.0); //rojo, verde, azul, opacidad
	vColor.rgb = scene_ambient + acumulador_difuso + acumulador_especular;
	//RGB = color, a= 1.0

	//Añadir texturas (color * coordenada de textura)
	vec4 texColor;
	texColor = texture2D(texture0, f_texCoord);


	gl_FragColor = vColor * texColor;


}
