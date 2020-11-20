#include <vector>
#include <cstdint>

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

#endif
