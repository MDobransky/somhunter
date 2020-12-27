//g++ -std=c++14 extract_features.cpp -I ../libtorch/include -I ../libtorch/include/torch/csrc/api/include -L ../libtorch/lib -lc10 -ltorch -ltorch_cpu -D_GLIBCXX_USE_CXX11_ABI=0 `pkg-config --cflags --libs opencv` -o extract_features
//g++-8 -std=c++14 extract_features.cpp -D_GLIBCXX_USE_CXX11_ABI=1 -I libtorch/include  
//       -I libtorch/include/torch/csrc/api/include -L libtorch/lib -lc10 -ltorch -ltorch_cpu 
//      -lstdc++fs `pkg-config --cflags --libs opencv` -o extract_features


#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <experimental/filesystem>


namespace fs = std::experimental::filesystem;

std::vector<float> parse_float_vector(const std::string &filepath,
                                  size_t dim,
                                  size_t begin_offset = 0)
{
	// Open file for reading as binary from the end side
	std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

	// If failed to open file
	if (!ifs)
		throw std::runtime_error("Error opening file: " + filepath);

	// Get end of file
	auto end = ifs.tellg();

	// Get iterator to begining
	ifs.seekg(0, std::ios::beg);

	// Compute size of file
	auto size = std::size_t(end - ifs.tellg());

	// If emtpy file
	if (size == 0) {
		throw std::runtime_error("Empty file opened!");
	}

	// Calculate byte length of each row (dim_N * sizeof(float))
	size_t row_byte_len = dim * sizeof(float);

	// Create line buffer
	std::vector<char> line_byte_buffer;
	line_byte_buffer.resize(row_byte_len);

	// Start reading at this offset
	ifs.ignore(begin_offset);

	// Initialize vector of floats for this row
	std::vector<float> features_vector;
	features_vector.reserve(dim);

	// Read binary "lines" until EOF
	while (ifs.read(line_byte_buffer.data(), row_byte_len)) {
		size_t curr_offset = 0;

		// Iterate through all floats in a row
		for (size_t i = 0; i < dim; ++i) {
			features_vector.emplace_back(*reinterpret_cast<float *>(
			  line_byte_buffer.data() + curr_offset));

			curr_offset += sizeof(float);
		}

		// Read just one line
		break;
	}

	return features_vector;
}

std::vector<std::vector<float>> parse_float_matrix(const std::string &filepath,
                                  size_t row_dim,
                                  size_t begin_offset = 0)
{
	// Open file for reading as binary from the end side
	std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

	// If failed to open file
	if (!ifs) {
		throw std::runtime_error("Error opening file: " + filepath);
	}

	// Get end of file
	auto end = ifs.tellg();

	// Get iterator to begining
	ifs.seekg(0, std::ios::beg);

	// Compute size of file
	auto size = std::size_t(end - ifs.tellg());

	// If emtpy file
	if (size == 0) {
		throw std::runtime_error("Empty file opened!");
	}

	// Calculate byte length of each row (dim_N * sizeof(float))
	size_t row_byte_len = row_dim * sizeof(float);

	// Create line buffer
	std::vector<char> line_byte_buffer;
	line_byte_buffer.resize(row_byte_len);

	// Start reading at this offset
	ifs.ignore(begin_offset);

	// Declare result structure
	std::vector<std::vector<float>> result_features;

	// Read binary "lines" until EOF
	while (ifs.read(line_byte_buffer.data(), row_byte_len)) {
		// Initialize vector of floats for this row
		std::vector<float> features_vector;
		features_vector.reserve(row_dim);

		size_t curr_offset = 0;

		// Iterate through all floats in a row
		for (size_t i = 0; i < row_dim; ++i) {
			features_vector.emplace_back(*reinterpret_cast<float *>(
			  line_byte_buffer.data() + curr_offset));

			curr_offset += sizeof(float);
		}

		// Insert this row into the result
		result_features.emplace_back(std::move(features_vector));
	}

	return result_features;
}

int main(int argc, const char* argv[]) {

    torch::jit::script::Module resnet152;
    try 
    {
        resnet152 = torch::jit::load("models/traced_Resnet152.pt");
    }
    catch (const c10::Error& e) 
    {
        std::cerr << "error loading the model\n";
        return -1;
    }

    torch::jit::script::Module resnext101;
    try 
    {
        resnext101 = torch::jit::load("models/traced_Resnext101.pt");
    }
    catch (const c10::Error& e) 
    {
        std::cerr << "error loading the model\n";
        return -1;
    }

    int height = 180;
    int width = 320;

    std::vector<cv::Rect> RoIs = {
            cv::Rect(0.00 * width, 0.0 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.25 * width, 0.0 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.50 * width, 0.0 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.75 * width, 0.0 * height, 0.25 * width, 0.5 * height),

            cv::Rect(0.00 * width, 0.5 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.25 * width, 0.5 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.50 * width, 0.5 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.75 * width, 0.5 * height, 0.25 * width, 0.5 * height), 

            cv::Rect(0.00 * width, 0.0 * height, 1.0 * width, 1.0 * height),
            cv::Rect(0.125 * width, 0.25 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.375 * width, 0.25 * height, 0.25 * width, 0.5 * height),
            cv::Rect(0.625 * width, 0.25 * height, 0.25 * width, 0.5 * height),
            }; 

    auto bias = torch::tensor(parse_float_vector("extractor/models/w2vv-img_bias-2048floats.bin", 2048));
    auto weights = torch::tensor(parse_float_vector("extractor/models/w2vv-img_weight-2048x4096floats.bin", 4096*2048)).reshape({2048, 4096}).permute({1, 0});

    for(auto& p: fs::directory_iterator("data/test"))
    {
        for(auto& f: fs::directory_iterator(p))
        {
            std::cout << f << '\n';
            cv::Mat src = cv::imread(f.path().u8string());

            float means[] = {123.68, 116.779, 103.939}; 
            std::vector<torch::Tensor> batch;
            std::vector<torch::Tensor> batch_norm;

            for(int i = 0; i < RoIs.size(); i++)
            {
                cv::Mat region = src(RoIs[i]);
                cv::resize(region, region, cv::Size(50, 50));

                auto tensor_image = torch::from_blob(region.data, {region.rows, region.cols, region.channels() }, at::kByte).to(torch::kFloat);
                
                torch::Tensor t_means = torch::from_blob(means, {3}).unsqueeze_(0).unsqueeze_(0);

                auto tensor_image_norm = tensor_image - t_means;

                tensor_image = tensor_image.permute({ 2,0,1 });
                tensor_image_norm = tensor_image_norm.permute({ 2,0,1 });
                
                tensor_image.unsqueeze_(0);
                tensor_image_norm.unsqueeze_(0);

                batch.push_back(tensor_image);
                batch_norm.push_back(tensor_image_norm);

            }  

            auto batch_of_tensors = torch::cat(batch);
            auto batch_of_tensors_norm = torch::cat(batch_norm);

            at::Tensor resnext101_feature = resnext101.forward({batch_of_tensors_norm}).toTensor();
            at::Tensor resnet152_feature = resnet152.forward({batch_of_tensors}).toTensor();

            at::Tensor feature = torch::cat({resnext101_feature, resnet152_feature}, 1).to(torch::kFloat32).detach();
			feature = feature.unsqueeze(0).permute({1, 0, 2});
			feature =  torch::tanh(torch::matmul(feature, weights).squeeze(1) + bias);

        }

        

    }

}
