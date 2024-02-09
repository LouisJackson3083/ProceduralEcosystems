#include "Noise.h"


Noise::Noise(float input_scale, int input_octaves, float input_persistance, float input_lacunarity, int input_seed) {
    scale = input_scale;
    octaves = input_octaves;
    persistance = input_persistance;
    lacunarity = input_lacunarity;
    seed = input_seed;
    srand(seed);
    time_created = time(0);

    fastNoiseLite.SetSeed(seed);
    fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);

    float amplitude = 1.0f;
    float frequency = 1;
    predictedNoiseMax = 0.0f;
    for (int i = 0; i < octaves; i++) {
        predictedNoiseMax += 1.0f * amplitude;
        amplitude *= persistance;
        frequency *= lacunarity;

        offsets.push_back(rand());
    }



}

void Noise::updateNoiseValues(float input_scale, int input_octaves, float input_persistance, float input_lacunarity) {
    scale = input_scale;
    octaves = input_octaves;
    persistance = input_persistance;
    lacunarity = input_lacunarity;

    float amplitude = 1.0f;
    float frequency = 1;
    predictedNoiseMax = 0.0f;
    for (int i = 0; i < octaves; i++) {
        predictedNoiseMax += 1.0f * amplitude;
        amplitude *= persistance;
        frequency *= lacunarity;
    }

    if (offsets.size() != octaves) {
        updateSeed(seed);
    }


}

float** Noise::generateNoiseMap(int width, int height, float scale) {
	float** noiseMap = new float* [height];

    if (scale <= 0) {
        scale = 0.000000001f;
    }

    for (int y = 0; y < height; y++) {

        noiseMap[y] = new float[width];

        for (int x = 0; x < width; x++) {
            float sampleX = x / scale;
            float sampleY = y / scale;

            //float simplexNoiseValue = SimplexNoise::noise(sampleX, sampleY);
            float simplexNoiseValue = fastNoiseLite.GetNoise((float)sampleX, (float)sampleY);


            noiseMap[y][x] = simplexNoiseValue * 255;
        }
    }

	return noiseMap;

}

float Noise::get(float x, float y, bool useErosion = false) {
    float amplitude = 1.0f;
    float frequency = 1;
    float noiseHeight = 0;
    float maxNoiseEstimate = 0.0f;
    float minNoiseEstimate = 0.0f;

    for (int i = 0; i < octaves; i++) {

        float sampleX = x / scale * frequency + offsets[i];
        float sampleY = y / scale * frequency + offsets[i];

        //float simplexNoiseValue = SimplexNoise::noise(sampleX, sampleY);
        float simplexNoiseValue = fastNoiseLite.GetNoise((float)sampleX, (float)sampleY);

        noiseHeight += simplexNoiseValue * amplitude;

        maxNoiseEstimate += 1.0f * amplitude;
        minNoiseEstimate -= 1.0f * amplitude;

        amplitude *= persistance;
        frequency *= lacunarity;
    }

    if (useErosion && 
        (x / scale > -(erosionMapSize / 2)) &&
        (y / scale > -(erosionMapSize / 2)) &&
        (x / scale < (erosionMapSize / 2)) &&
        (y / scale < (erosionMapSize / 2))
        ) {
        return ((noiseHeight - erosionMap[(int)(x/scale)+erosionMapSize/2][(int)(y/scale)+ erosionMapSize/2]) + predictedNoiseMax) / (predictedNoiseMax * 2);
    }
    else {
        return (noiseHeight + predictedNoiseMax) / (predictedNoiseMax * 2);
    }
}

void Noise::updateSeed(int input_seed) {
    seed = input_seed;
    srand(seed);

    offsets.clear();

    for (int i = 0; i < octaves; i++) {
        offsets.push_back(rand());
    }

    fastNoiseLite.SetSeed(seed);
}


void Noise::updateErosionValues(
    int input_numIterations, 
    int input_maxLifetime,
    float input_inertia,
    float input_depositSpeed,
    float input_erodeSpeed,
    float input_evaporateSpeed
) {
    numDroplets = input_numIterations;
    dropletLifetime = input_maxLifetime;
    inertia = input_inertia;
    depositSpeed = input_depositSpeed;
    erodeSpeed = input_erodeSpeed;
    evaporateSpeed = input_evaporateSpeed;
}

void Noise::generateErosionMap() {
    // Resize the erosion map and reset it
    erosionMap.clear();
    erosionMap.resize(erosionMapSize, std::vector<float>(erosionMapSize));
    std::for_each(erosionMap.begin(), erosionMap.end(), [](std::vector<float>& inner_vec)
        {
            std::fill(inner_vec.begin(), inner_vec.end(), 0.0f);
        });

    // numDroplets = number of water droplets to spawn and simulate
    for (int droplet = 0; droplet < numDroplets; droplet++) {
        // Create water droplet at random point on erosion map
        float x = rand() % erosionMapSize;
        float y = rand() % erosionMapSize;
        float directionX = 0; // stores the direction of the droplet in X
        float directionY = 0;
        float speed = 1.0f;
        float water = 1.0f;
        float sediment = 0.0f; // stores the amount of sediment the droplet gains as it falls down the terrain

        // For each stage of a droplets lifetime,
        for (int lifetime = 0; lifetime < dropletLifetime; lifetime++) {
            int intX = (int)x;
            int intY = (int)y;
            // Calculate droplet's offset within the vector cell so we can calculate more accurate erosion
            // (0,0) ----------- (1,0)
            // |                     |
            // |    o - (0.25, 0.25) |
            // |                     |
            // |                     |
            // |                     |
            // |                     |
            // (0,0) ----------- (1,1)
            float offsetX = x - intX;
            float offsetY = y - intY;

            // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
            // Where heightAndGradients[0] = height
            //       heightAndGradients[1] = gradientX
            //       heightAndGradients[2] = gradientY
            glm::vec3 heightAndGradients = ErosionGetHeightAndGradients(x, y);

            // Update the droplet's direction and position (move position 1 unit regardless of speed)
            directionX = (directionX * inertia - heightAndGradients[1] * (1 - inertia));
            directionY = (directionY * inertia - heightAndGradients[2] * (1 - inertia));

            // Normalize direction
            float len = sqrt(directionX * directionX + directionY * directionY);

            if (len != 0) {
                directionX /= len;
                directionY /= len;
            }
            x += directionX;
            y += directionY;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((directionX == 0 && directionY == 0) || x < 0 || x >= erosionMapSize - 1 || y < 0 || y >= erosionMapSize - 1) {
                break;
            }

            // Calculate the change in height by subtracting the new height from the old height
            float deltaHeight = heightAndGradients[0] - ErosionGetHeightAndGradients(x, y)[0];

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sedimentCapacity = std::max(-deltaHeight * speed * water * sedimentCapacityFactor, minSedimentCapacity);

            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sedimentCapacity || deltaHeight > 0) {
                // If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
                sediment -= amountToDeposit;

                // Add the sediment to the four nodes of the current cell using bilinear interpolation
                // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                int n = y - 1;
                int e = x + 1;
                int s = y + 1;
                int w = x - 1;
                // Stop simulating droplet if it's not moving or has flowed over edge of map
                if (n < 0 || n >= erosionMapSize - 1 || e < 0 || e >= erosionMapSize - 1 || s < 0 || s >= erosionMapSize - 1 || w < 0 || w >= erosionMapSize - 1) {
                    break;
                }
                else {
                    erosionMap[w][n] += (amountToDeposit * (1 - offsetX) * (1 - offsetY));
                    erosionMap[e][n] += amountToDeposit * offsetX * (1 - offsetY);
                    erosionMap[w][s] += amountToDeposit * (1 - offsetX) * offsetY;
                    erosionMap[e][s] += amountToDeposit * offsetX * offsetY;
                }
            }
            else {
                // Erode a fraction of the droplet's current carry capacity.
                // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
                float amountToErode = std::min((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);
                int intX = (int)x;
                int intY = (int)y;
                if (intX > 0 && intX < erosionMapSize && intY > 0 && intY < erosionMapSize) {
                    erosionMap[(int)x][(int)y] += amountToErode;
                }
            }

            // Update droplet's speed and water content
            speed = sqrt(speed * speed + deltaHeight * -9.81f);
            water *= (1 - evaporateSpeed);
        }
    }
}
glm::vec3 Noise::ErosionGetHeightAndGradients(float x, float y) {
    int intX = (int)x;
    int intY = (int)y;
    // Calculate droplet's offset within the vector cell so we can calculate more accurate erosion
    // (0,0) ----------- (1,0)
    // |                     |
    // |    o - (0.25, 0.25) |
    // |                     |
    // |                     |
    // |                     |
    // |                     |
    // (0,0) ----------- (1,1)
    float offsetX = x - intX;
    float offsetY = y - intY;

    // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
    float heightNW = get(intX - 1 - (erosionMapSize / 2), intY - 1 - (erosionMapSize / 2));
    float heightNE = get(intX + 1 - (erosionMapSize / 2), intY - 1 - (erosionMapSize / 2));
    float heightSW = get(intX - 1 - (erosionMapSize / 2), intY + 1 - (erosionMapSize / 2));
    float heightSE = get(intX + 1 - (erosionMapSize / 2), intY + 1 - (erosionMapSize / 2));

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradientX = (heightNE - heightNW) * (1 - offsetY) + (heightSE - heightSW) * offsetY;
    float gradientY = (heightSW - heightNW) * (1 - offsetX) + (heightSE - heightNE) * offsetX;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height =
        heightNW * (1 - offsetX) * (1 - offsetY) +
        heightNE * offsetX * (1 - offsetY) +
        heightSW * (1 - offsetX) * offsetY +
        heightSE * offsetX * offsetY;

    return glm::vec3(height, gradientX, gradientY);
}