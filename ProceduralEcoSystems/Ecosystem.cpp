#include "Ecosystem.h"

Ecosystem::Ecosystem(int id) {
	layerRadii = { 30.0f, 20.0f, 15.0f };
}

void Ecosystem::GeneratePoissonPositions() {
	constexpr auto kXMin = std::array<float, 2>{{-128.0f, -128.0f}};
	constexpr auto kXMax = std::array<float, 2>{{127.0f, 127.0f}};

	// Minimal amount of information provided to sampling function.
	for (int i = 0; i < 3; i++) {
		const auto samples = thinks::PoissonDiskSampling(layerRadii[i], kXMin, kXMax);

		/*class std::array<float, 2>
		class std::vector<class std::array<float, 2>, class std::allocator<class std::array<float, 2> > >*/

		std::vector<glm::vec2> currPositions;

		for (const auto& sample : samples) {
			currPositions.push_back(glm::vec2(sample[0], sample[1]));
		}
		poissonPositions.push_back(currPositions);
	}
}