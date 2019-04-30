#version 430
// Define the size of the local work group
layout(local_size_x = 1, local_size_y = 1) in;
// Define the internal format of the image that we set up. image2D uniform is used instead of a 
// texture sample to give us the ability to write to any pixel that we want.
layout(rgba32f, binding = 0) uniform image2D img_output;

void main() {
	// Base pixel colour for image.
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
	// Get index in global work group i.e x,y position.
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	// --- Ray ---
	// Has origin O.
	// Has direction normal D.
	// Any xyz point on the ray can be define as a function of its distance, t, from the origin:
	//	R(t) = O + D*t
	// if t < 0, it is behind O.
	float max_x = 5.0;
	float max_y = 5.0;
	ivec2 dims = imageSize(img_output);	// Fetch image dimensions.
	float x = (float(pixel_coords.x * 2 - dims.x) / dims.x);
	float y = (float(pixel_coords.y * 2 - dims.y) / dims.y);
	vec3 ray_o = vec3(x * max_x, y * max_y, 0.0);	// Ray origin
	vec3 ray_d = vec3(0.0, 0.0, -1.0);				// Ray direction. 0 because orthgraphic projection.
	// --- End Ray ---

	// --- Sphere ---
	vec3 sphere_c = vec3(0.0, 0.0, -10.0);	// Center of the sphere.
	float sphere_r = 1.0;					// Sphere radius.
	// --- End Sphere ---

	// --- Ray-sphere intersection test ---
	// All points on sphere and ray:
	//	|| O+Dt-C || - r = 0
	// This rearranges into a quadratic:
	//	t^2 + 2t + b + c = 0
	// Where:
	//	b = D * (0 - C)
	//	c = (0 - C) * (0 - C) - r^2
	// Case 1: b^2 - c < 0: Miss.
	// Case 2: b^2 - c > 0: Through the sphere.
	// Case 3: b^2 - c = 0: Tangante on the sphere.
	vec3 omc = ray_o - sphere_c;
	float b = dot(ray_d, omc);
	float c = dot(omc, omc) - sphere_r * sphere_r;
	float bsqmc = b * b - c;
	// Hit one or both sides?
	if (bsqmc >= 0.0) {
		pixel = vec4(1.0, 0.0, 0.0, 1.0);
	}

	// Output to a specific pixel in the image.
	imageStore(img_output, pixel_coords, pixel);
}