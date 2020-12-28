
/* This file is part of SOMHunter.
 *
 * Copyright (C) 2020 František Mejzlík <frankmejzlik@gmail.com>
 *                    Mirek Kratochvil <exa.exa@gmail.com>
 *                    Patrik Veselý <prtrikvesely@gmail.com>
 *
 * SOMHunter is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * SOMHunter is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * SOMHunter. If not, see <https://www.gnu.org/licenses/>.
 */

#include "SomHunter.h"
#include <chrono>
#include <stdio.h>
#include <thread>

#if 0
void
print_display(const FramePointerRange &d)
{
	for (auto iter = d.begin(); iter != d.end(); iter++)
		std::cout << (*iter)->frame_ID << std::endl;
}
#endif

int
main()
{
	// Assuming root of your project is `build/src/`...
	// Change this accordingly
	const std::string cfg_fpth{ "../../config.json" };

	// Parse config file
	auto config = Config::parse_json_config(cfg_fpth);

	// Instantiate the SOMHunter
	SomHunter core{ config };

	// Test features here...
	/*
	 * Test ImageManipulator
	 */
	std::string orig_img{ "testimg.jpg" };
	auto img{ core.load_image(orig_img) };

	for (size_t i{ 0 }; i < 10; ++i) {
		size_t new_w{ irand(10_z, 2000_z) };
		size_t new_h{ irand(10_z, 2000_z) };

		std::string new_file{ "testimg_edited_" + std::to_string(i) + ".jpg" };

		auto tmp_data{ core.resize_image(img.data, img.w, img.h, new_w, new_h, img.num_channels) };
		core.store_jpg_image(new_file, tmp_data, new_w, new_h, 50, img.num_channels);

		// Check!
		auto tmp_img{ core.load_image(new_file) };

		if (tmp_img.w != new_w)
			throw std::logic_error("Width does not match!");

		if (tmp_img.h != new_h)
			throw std::logic_error("Height does not match!");
	}

#if 0
	debug("this is debug log");
	info("this is info log");
	warn("this is warn log");
	

	// Try autocomplete
	auto ac_res{ core.autocomplete_keywords("cat", 30) };
	for (auto &&p_kw : ac_res) {
		std::cout << p_kw->synset_strs.front() << std::endl;
	}

	// Try keyword rescore
	core.rescore("dog park");

	// Try different displays
	auto d_topn = core.get_display(DisplayType::DTopN, 0, 0);
	std::cout << "TOP N\n";
	print_display(d_topn);

	auto d_topknn = core.get_display(DisplayType::DTopKNN, 2, 0);
	std::cout << "TOP KNN\n";
	print_display(d_topknn);

	auto d_rand = core.get_display(DisplayType::DRand);
	std::cout << "RANDOM\n";
	print_display(d_rand);

	// Try keyword rescore
	core.rescore("dog park");

	d_topn = core.get_display(DisplayType::DTopN, 0, 0);
	std::cout << "TOP N\n";
	print_display(d_topn);

	// Try reset session
	core.reset_search_session();

	// Try relevance feedback
	d_rand = core.get_display(DisplayType::DRand);
	std::vector<ImageId> likes;
	auto d_rand_b = d_rand.begin();
	likes.push_back((*d_rand_b)->frame_ID);
	d_rand_b++;
	likes.push_back((*d_rand_b)->frame_ID);

	core.add_likes(likes);

	likes.resize(1);

	core.remove_likes(likes);

	std::cout << "Like " << likes[0] << std::endl;

	core.rescore("\\/?!,.'\"");

	d_topn = core.get_display(DisplayType::DTopN, 0, 0);
	std::cout << "TOP N\n";
	print_display(d_topn);

	std::cout << "Len of top n page 0 " << d_topn.size() << std::endl;
	d_topn = core.get_display(DisplayType::DTopN, 0, 1);
	std::cout << "Len of top n page 1 " << d_topn.size() << std::endl;
	d_topn = core.get_display(DisplayType::DTopN, 0, 2);
	std::cout << "Len of top n page 2 " << d_topn.size() << std::endl;

	// Try SOM
	while (!core.som_ready()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cout << "SOM is ready now!" << std::endl;

	auto d_som = core.get_display(DisplayType::DSom);
#endif
	return 0;
}
