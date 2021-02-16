layout (std140) uniform Test {
	int i;
	struct t2 {
		float f;
		vec2 v2;
	};
	struct t3 {
		mat3 m;
		vec3 v;
	} a[2];
};