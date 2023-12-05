#ifndef NOISE_CLASS_H
#define NOISE_CLASS_H

class Noise {
public:
	float** noiseMap;

	Noise();

	float** generateNoiseMap(int width, int height, float scale);

	void SaveImage(const char* filename, float img_vals[][512]);


private:

};

#endif