uniform vec3 light;
uniform float r;
uniform sampler2D texsample;

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal

void main (void) {
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);

	float f1 = max(dot(normalize(light - p), n), 0.005);
	float f2 = clamp(pow(abs(dot(n, normalize(-p))), r), 0.005, 0.995);
	gl_FragColor = texture2D(texsample, vec2(f1, 1. - f2)); 
}