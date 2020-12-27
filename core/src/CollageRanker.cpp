#include "CollageRanker.h"

CollageRanker::CollageRanker()
{
    try 
    {
        resnet152 = torch::jit::load("models/traced_Resnet152.pt");
    }
    catch (const c10::Error& e) 
    {
        std::cerr << "error loading the resnet152 model\n";
    }

    try 
    {
        resnext101 = torch::jit::load("models/traced_Resnext101.pt");
    }
    catch (const c10::Error& e) 
    {
        std::cerr << "error loading the resnext101 model\n";
    }
}

void CollageRanker::score(const Collage& collage)
{
    // - np.array([[[123.68, 116.779, 103.939]]]
    //reshape 224x224
    //at::Tensor masks_a = at::upsample_bilinear2d(masks.unsqueeze(0), {img_size, img_size}, false);


    std::vector<torch::Tensor> batch;


    for(auto &c : collage.p_widths)
    {
        debug_l(c << "\n");
    }
}


at::Tensor CollageRanker::get_features(const at::Tensor& batch_of_tensors)
{
    at::Tensor resnet152_feature = resnet152.forward({batch_of_tensors}).toTensor();
    at::Tensor resnext101_feature = resnext101.forward({batch_of_tensors}).toTensor();
    at::Tensor feature = torch::cat({resnet152_feature, resnext101_feature}, 1).to(torch::kFloat32).detach();
}