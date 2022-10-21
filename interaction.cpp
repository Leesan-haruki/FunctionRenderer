/*
    Behavior after moving the mouse is set as OpenCV's mouse callback fucntion

    left-click: rotating the camera origin around the camera destination according to Arcball operation
    shift-key + left-click: scaling, expanding when up the mouse and shrinking when down
    right-click: translating the camera

    Behavior while moving the mouse will be implemented as low resolution preview
*/

#include "interaction.h"

bool translating = false;
bool scaling = false;
bool rotating = false;
int ix = -1; int iy = -1;

Vector3 rotate_any_axis(const Vector3 &n, const double angle, const Vector3 &v){
    // Rodrigues' rotation formula
    Vector3 R1 = Vector3(
        n.x * n.x * (1 - std::cos(angle)) + std::cos(angle),
        n.x * n.y * (1 - std::cos(angle)) - n.z * std::sin(angle),
        n.x * n.z * (1 - std::cos(angle)) + n.y * std::sin(angle)
    );
    Vector3 R2 = Vector3(
        n.y * n.x * (1 - std::cos(angle)) + n.z * std::sin(angle),
        n.y * n.y * (1 - std::cos(angle)) + std::cos(angle),
        n.y * n.z * (1 - std::cos(angle)) - n.x * std::sin(angle)
    );
    Vector3 R3 = Vector3(
        n.z * n.x * (1 - std::cos(angle)) - n.y * std::sin(angle),
        n.z * n.y * (1 - std::cos(angle)) + n.x * std::sin(angle),
        n.z * n.z * (1 - std::cos(angle)) + std::cos(angle)
    );
    return Vector3(R1.dot(v), R2.dot(v), R3.dot(v));
}

void changePerspective(int event, int x, int y, int flag, void* userdata){
    TracerData* td = (TracerData*)userdata;

    // translation
    if(event == cv::EVENT_RBUTTONDOWN){
        translating = true; ix = x; iy = y;
    }
    else if(event == cv::EVENT_RBUTTONUP && translating){
        double dx = (double)(x - ix) / td->img->rows;
        double dy = (double)(y - iy) / td->img->cols;

        Vector3 v_view = camera_t - camera_o;
        Vector3 v_right = (v_view.x != 0 || v_view.y != 0) ? Vector3(-v_view.y, v_view.x, 0).normalize() : Vector3(1, 0, 0);
        Vector3 v_up = v_right.cross(v_view).normalize();
        camera_o -= v_right * dx + v_up * dy;
        camera_t -= v_right * dx + v_up * dy;

        render(*(td->img), *(td->model), *(td->matcap_img), false);
        translating = false; ix = -1; iy = -1;
    }
    // scale
    else if(event == cv::EVENT_LBUTTONDOWN && (flag & cv::EVENT_FLAG_SHIFTKEY)){
        scaling = true; iy = y;
    }
    else if(event == cv::EVENT_LBUTTONUP && (flag & cv::EVENT_FLAG_SHIFTKEY) && scaling){
        double dy = (double)(y - iy) / td->img->cols;

        Vector3 v_view = camera_t - camera_o;
        camera_t += v_view * dy;

        render(*(td->img), *(td->model), *(td->matcap_img), false);
        scaling = false; iy = -1;
    }
    // rotate
    else if(event == cv::EVENT_LBUTTONDOWN){
        rotating = true; ix = x; iy = y;
    }
    else if(event == cv::EVENT_LBUTTONUP && rotating){
        double x0 = (double)ix; double y0 = (double)iy;
        double x1 = (double)x; double y1 = (double)y;
        double W = td->img->cols; double H = td->img->rows;
        if((x - ix) * (x - ix) + (y - iy) * (y - iy) <= 100) return; 

        Vector3 v0 = Vector3(2 * x0 / W - 1.0, -(2 * y0 / H - 1.0), 0);
        Vector3 v1 = Vector3(2 * x1 / W - 1.0, -(2 * y1 / H - 1.0), 0);
        if(v0.x * v0.x + v0.y * v0.y < 1.0) v0.z = std::sqrt(1.0 - (v0.x * v0.x + v0.y * v0.y));
        else v0 = v0.normalize();
        if(v1.x * v1.x + v1.y * v1.y < 1.0) v1.z = std::sqrt(1.0 - (v1.x * v1.x + v1.y * v1.y));
        else v1 = v1.normalize();

        Vector3 v_view = (camera_t - camera_o).normalize();
        Vector3 v_right = (v_view.x != 0 || v_view.y != 0) ? Vector3(-v_view.y, v_view.x, 0).normalize() : Vector3(1, 0, 0);
        Vector3 v_up = v_right.cross(v_view).normalize();
        // Vector3 v0 = (v_right * v0.x + v_up * v0.y + v_view * v0.z).normalize();
        // Vector3 v1 = (v_right * v1.x + v_up * v1.y + v_view * v1.z).normalize();
        v0.print(); v1.print();
        std::cout << "dot: " << v0.dot(v1) << std::endl;
        double alpha = std::acos(v0.dot(v1));
        Vector3 axis_ = v0.cross(v1).normalize();
        Vector3 axis = (v_right * v0.x + v_up * v0.y + v_view * v0.z).normalize();
        std::cout << "angle: " << alpha << std::endl; axis_.print(); axis.print();
        Vector3 rotate_v = rotate_any_axis(axis, alpha, (camera_o - camera_t));
        camera_o = camera_t + rotate_v;

        render(*(td->img), *(td->model), *(td->matcap_img), true);
        rotating = false; ix = -1; iy = -1;
    }

    // else if(event == cv::EVENT_MOUSEMOVE){
    //     if(translating){

    //     }
    //     if(scaling){

    //     }
    //     if(rotating){

    //     }
    // }
}
