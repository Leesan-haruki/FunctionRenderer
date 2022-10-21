#include <algorithm>
#include <assert.h>
#include <string>
#include <utility>
#include <vector>

#include <opencv4/opencv2/opencv.hpp>

#include "boolean.h"
#include "interaction.h"
#include "model.h"
#include "render.h"
#include "vector3.h"

void init_camera(void){
    camera_o = Vector3(3, 3, 3);
    camera_t = Vector3(0, 0, 0);
}

int main(int argc, char **argv){
    int width, height;
    if(argc == 3){
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    else if(argc == 2){
        width = std::stoi(argv[1]);
        height = std::stoi(argv[1]);
    }
    else{
        width = 512; height = 512;
    }

    cv::Mat image(cv::Size(width, height), CV_8UC3);
    const std::string matcap_path = "matcap/green.png";  // Takikawa et al. 2021
    cv::Mat matcap_img = cv::imread(matcap_path, -1);
    assert(!matcap_img.empty());

    init_camera();

    // model creation TODO: separate function
    // Model model;
    // Torus torus = Torus(1, 0.3);
    Sphere sphere = Sphere(Vector3(0, 0, 0), 1);
    Rectangular rect = Rectangular(Vector3(0, 0, 0), 0.5, 1, 1.5);
    // Quadric quad = Quadric(1, 2, 3, 0, 0, 0, 0, 0, 0, -1);
    Difference model = Difference(&rect, &sphere);

    TracerData tracer_data = { &image, &model, &matcap_img };
    render(image, model, matcap_img, true);

    cv::imwrite("result/result.png", image);
    cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Result", changePerspective, (void*)&tracer_data);
    while(1){
        cv::imshow("Result", image);
        int key = cv::waitKey(1) & 0xFF;
        if(key == 27) break;
        if(key == 114){ // press 'r'
            init_camera();
            render(image, model, matcap_img, true);
        }
        if(key == 115){ // press 's'
            cv::imwrite("result/result0.png", image);
        }
    }
    cv::destroyWindow("Result");
    return 0;
}
