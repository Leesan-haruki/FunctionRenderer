#include <opencv4/opencv2/opencv.hpp>

#include "vector3.h"
#include "model.h"
#include "render.h"

#ifndef _INTERACTION_H_
#define _INTERACTION_H_

struct TracerData{
    cv::Mat* img;
    Model* model;
    cv::Mat* matcap_img;
};

Vector3 rotate_any_axis(const Vector3 &n, const double angle, const Vector3 &v);

void changePerspective(int event, int x, int y, int flag, void* userdata);

#endif
