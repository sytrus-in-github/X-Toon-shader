uniform vec3 light;
uniform float zfoc;
uniform float zmin;
uniform float zmax;
uniform sampler2D texsample;

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal

float getFocus(float z){
	if (z > zfoc + zmin){
		return log(z / (zfoc + zmax)) / log((zfoc + zmin) / (zfoc + zmax));
	}
	else if(z < zfoc - zmin){
		return 1. - log(z / (zfoc - zmin)) / log((zfoc - zmax) / (zfoc - zmin));
	}
	else {
		return 1.;
	}
}

void main (void) {
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);

	float z = clamp(length(p),zfoc-zmax+0.005,zfoc+zmax-0.02);
	float f1 = max(dot(normalize(light - p), n), 0.005);
	float f2 = min(getFocus(z), 0.995);
	gl_FragColor = texture2D(texsample, vec2(f1, 1.-f2));
}

