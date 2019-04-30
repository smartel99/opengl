#include "Utils.h"
#include <cmath>

void HSVtoRGB(int H, float S, float V, float* r, float* g, float* b)
{
	float C = S * V;
	float X = C * (1 - float(abs(fmod(H / 60.0, 2) - 1)));
	float m = V - C;
	float Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	*r = (Rs + m);
	*g = (Gs + m);
	*b = (Bs + m);
}
