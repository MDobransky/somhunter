#include <vector>
#include <cstdint>
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "log.h"

#ifndef COLLAGE
#define COLLAGE

class Collage
{
public:

    std::vector<float> lefts;
	std::vector<float> tops;
	std::vector<float> heights;
	std::vector<float> widths;
	std::vector<unsigned int> p_heights;
	std::vector<unsigned int> p_widths;
	std::vector<std::vector<uint8_t>> images;
    int break_point;

};

class CollageRanker
{
	public:
		CollageRanker();
		void score(const Collage&);

	private:
		torch::jit::script::Module resnet152;
		torch::jit::script::Module resnext101;

		at::Tensor get_features(const at::Tensor&);
};

#endif
