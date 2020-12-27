{
    "targets": [
        {
            "target_name": "somhunter_core",
            "sources": [
                "main.cpp",
                "SomHunterNapi.cpp",
                "src/json11.cpp",
                "src/SomHunter.cpp",
                "src/SOM.cpp",
                "src/AsyncSom.cpp",
                "src/CollageRanker.cpp",
                "src/DatasetFeatures.cpp",
                "src/DatasetFrames.cpp",
                "src/KeywordRanker.cpp",
                "src/RelevanceScores.cpp",
                "src/Submitter.cpp",
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "src/",
                "../libtorch/include",
                "../libtorch/include/torch/csrc/api/include"
                
            ],
            "libraries": ["-L ../libtorch/lib", "-lc10", "-ltorch", "-ltorch_cpu", "-lstdc++fs"],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            "defines": [
                "NAPI_CPP_EXCEPTIONS",
                "HAS_NAPI_HEADERS"
            ],
            "cflags_cc": [
                "-std=c++17","-fexceptions", "-march=native", "-frtti", "-D_GLIBCXX_USE_CXX11_ABI=1"
            ],
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "AdditionalOptions": [
                        "-std:c++17"
                    ]
                }
            },
            "conditions": [
                [
                    "OS=='linux'",
                    {
                        "include_dirs":[
                            "<!@(pkg-config libcurl --cflags-only-I | sed s/-I//g)"
                        ],
                        "link_settings": {
                            "libraries": [
                                "-L/usr/lib64/",
                                "<!@(pkg-config libcurl --libs)"
                            ]
                        }
                    }
                ],
                [
                    "OS=='win'",
                    {
                        "include_dirs":[
                            "C:\\Program Files\\curl\\include\\"
                        ],
                        "link_settings": {
                            "libraries": [
                                "libcurl.lib",
                                "zlib.lib"
                            ]
                        },
                        "copies": [
                            {
                                "destination": "<(PRODUCT_DIR)",
                                "files": [ "C:\\Program Files\\curl\\bin\\libcurl.dll" ]
                            }
                        ],
                        "msvs_settings": {
                            "VCCLCompilerTool": {
                                "AdditionalOptions": [
                                    "-std:c++17",
                                    "/MP /EHsc /Qspectre"
                                ]
                            },
                            "VCLinkerTool": {
                                "AdditionalLibraryDirectories": [
                                    "C:\\Program Files\\curl\\lib\\"
                                ]
                            }
                        }
                    }
                ]
            ]
        }
    ]
}
