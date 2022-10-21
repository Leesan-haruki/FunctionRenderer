#include <opencv4/opencv2/opencv.hpp>

#include "model.h"

#ifndef _RENDER_H_
#define _RENDER_H_

extern Vector3 camera_o;
extern Vector3 camera_t;

void decide_ray_direction(std::vector<std::vector<Vector3>> &ray_d, int width, int height);

void sphere_tracing(Model &model, std::vector<std::vector<Vector3>> &ray_d, std::vector<std::vector<Vector3>> &nrms, 
    std::vector<std::vector<bool>> &collided, int width, int height);

void matcap_texture(cv::Mat &image, cv::Mat &matcap_img, std::vector<std::vector<Vector3>> &ray_d,
    std::vector<std::vector<Vector3>> &nrms, std::vector<std::vector<bool>> &collided, int width, int height);

void render(cv::Mat &image, Model &model, cv::Mat &matcap_img, bool logger);

#endif
