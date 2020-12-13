//g++ -std=c++14 cpp_inference.cpp -I ../libtorch/include -I ../libtorch/include/torch/csrc/api/include -L ../libtorch/lib -lc10 -ltorch -ltorch_cpu -D_GLIBCXX_USE_CXX11_ABI=0 `pkg-config --cflags --libs opencv` -o cpp_inference

#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <opencv4/opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <experimental/filesystem>


namespace fs = std::experimental::filesystem;

void save(const std::string &file_name,cv::PCA pca_)
{
    cv::FileStorage fs(file_name, cv::FileStorage::WRITE);
    fs << "mean" << pca_.mean;
    fs << "e_vectors" << pca_.eigenvectors;
    fs << "e_values" << pca_.eigenvalues;
    fs.release();
}

int load(const std::string &file_name, cv::PCA pca_)
{
    cv::FileStorage fs;
    fs.open(file_name, cv::FileStorage::READ);
    cv::FileNode n = fs.root();
    fs["mean"] >> pca_.mean;
    fs["e_vectors"] >> pca_.eigenvectors;
    fs["e_values"] >> pca_.eigenvalues;
    fs.release();

    std::cout << pca_.eigenvalues << std::endl;

}

int main(int argc, const char* argv[]) {

    torch::jit::script::Module resnet152;
        try {
        resnet152 = torch::jit::load("models/traced_Resnet152.pt");
    }
        catch (const c10::Error& e) {
        std::cerr << "error loading the model\n";
        return -1;
    }

    torch::jit::script::Module resnext101;
        try {
        resnext101 = torch::jit::load("models/traced_Resnext101.pt");
    }
        catch (const c10::Error& e) {
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


    for(auto& p: fs::directory_iterator("data/test"))
    {
        for(auto& f: fs::directory_iterator(p))
        {
            std::cout << f << '\n';
            cv::Mat src = cv::imread(f.path().u8string());

            float means[] = {123.68, 116.779, 103.939}; 
            std::vector<torch::Tensor> batch;

            for(int i = 0; i < RoIs.size(); i++)
            {
                cv::Mat region = src(RoIs[i]);
                cv::resize(region, region, cv::Size(50, 50));

                auto tensor_image = torch::from_blob(region.data, { region.rows, region.cols, region.channels() }, at::kByte);

                
                torch::Tensor t_means = torch::from_blob(means, {3}).unsqueeze_(0).unsqueeze_(0);

                tensor_image = tensor_image - t_means;

                tensor_image = tensor_image.permute({ 2,0,1 });
                tensor_image.unsqueeze_(0);

                batch.push_back(tensor_image);

            }  

            auto batch_of_tensors = torch::cat(batch);

            at::Tensor resnet152_feature = resnet152.forward({batch_of_tensors}).toTensor();
            at::Tensor resnext101_feature = resnext101.forward({batch_of_tensors}).toTensor();
            at::Tensor feature = torch::cat({resnet152_feature, resnext101_feature}, 1).to(torch::kFloat32).detach();

            cv::Mat mdata(feature.size(0), feature.size(1), CV_32FC1, feature.data_ptr<float>());
            std::cout << mdata.size() << std::endl;

            cv::PCA pca(mdata, cv::Mat(), cv::PCA::DATA_AS_ROW, 256);

            std::cout << pca.eigenvalues << std::endl;

            save("pca.yaml", pca);

            
            cv::PCA pcal;
            load("pca.yaml", pcal);


            std::cout << pcal.eigenvalues << std::endl;

            


            // float X_array[]={2.5,0.5,2.2,1.9,3.1,2.3,2,1,1.5,1.1};
            // float Y_array[]={2.4,0.7,2.9,2.2,3.0,2.7,1.6,1.1,1.6,0.9};

            // cv::Mat x(10,1,CV_32F,X_array);   //Copy X_array to Mat (PCA need Mat form)
            // cv::Mat y(10,1,CV_32F,Y_array);   //Copy Y_array to Mat

            // cv::Mat data(10, 2, CV_32F);
            // std::cout << data.size() << std::endl;


            // x.col(0).copyTo(data.col(0));  //copy x into first column of data
            // y.col(0).copyTo(data.col(1));  //copy y into second column of data


            // cv::PCA pca2(data,                 //Input Array Data
            // cv::Mat(),                //Mean of input array, if you don't want to pass it   simply put Mat()
            // CV_PCA_DATA_AS_ROW,   //int flag
            // 2);

            // std::cout << pca2.project(data.row(3)) << std::endl;

        }

    }

}
