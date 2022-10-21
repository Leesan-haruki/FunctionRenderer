/*
    Rendering of function-based model is implemented as this:
        1. decide each pixels ray direction : O(WH)
        2. Sphere Tracing from Hart 1995 to judge whether the ray hits the model or not and compute the normal at point when hitting
            : O(WHS) (S denotes the number of step)
        3. Matcap Texturing using ray direction and the normal of the model, with reference to Takikawa et al. 2021 : O(WH)
    These computation can be accelerated by parallel processing such as OpenMP or CUDA
*/

#include "render.h"

const int MAX_STEP = 100;
const double FINISH_MINIMUM = 0.001;
const double FINISH_MAXIMUM = 100;

Vector3 camera_o = Vector3(1, 1, 1);
Vector3 camera_t = Vector3(0, 0, 0);

void decide_ray_direction(std::vector<std::vector<Vector3>> &ray_d, int width, int height){
    Vector3 v_view = camera_t - camera_o;
    Vector3 v_right = (v_view.x != 0 || v_view.y != 0) ? Vector3(-v_view.y, v_view.x, 0).normalize() : Vector3(1, 0, 0);
    Vector3 v_up = v_right.cross(v_view).normalize();

    double real_h = 5;
    double real_w = real_h * width / height;
    for(int i = 0; i < height; i++){
        Vector3 h_translate = v_up * (real_h * (i-height/2) / height);
        for(int j = 0; j < width; j++){
            Vector3 w_translate = v_right * (real_w * (j-width/2) / width);
            ray_d[i][j] = ((camera_t + h_translate + w_translate) - camera_o).normalize();
        }
    }
}

void sphere_tracing(Model &model, std::vector<std::vector<Vector3>> &ray_d, std::vector<std::vector<Vector3>> &nrms, 
    std::vector<std::vector<bool>> &collided, int width, int height){
    // Sphere Tracer TODO: use cuda
    std::vector<std::vector<Vector3>> coords(height, std::vector<Vector3>(width, camera_o));
    std::vector<std::vector<double>> dists(height, std::vector<double>(width, 0));

    long total_step = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            // John C. Hart 1995
            double sdf = model.sdf(coords[i][j]);
            int step = 0;
            while(step < MAX_STEP && sdf > FINISH_MINIMUM && sdf < FINISH_MAXIMUM){
                // printf("\rtimes: %04d : %04d : %04d : %09ld", i, j, step, total_step);
                // fflush(stdout);
                coords[i][j] += ray_d[i][j] * sdf;
                dists[i][j] += sdf;
                sdf = model.sdf(coords[i][j]);
                step += 1;
                total_step += 1;
            }
            coords[i][j] += ray_d[i][j] * sdf;
            dists[i][j] += sdf;
            if(std::abs(sdf) <= FINISH_MINIMUM){
                collided[i][j] = true;
                nrms[i][j] = model.normal(coords[i][j]);
            }
        }
        
    }
    // printf("\n");
}

void matcap_texture(cv::Mat &image, cv::Mat &matcap_img, std::vector<std::vector<Vector3>> &ray_d,
    std::vector<std::vector<Vector3>> &nrms, std::vector<std::vector<bool>> &collided, int width, int height){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(collided[i][j]){
                // Takikawa et al. 2021
                Vector3 ray_d_sc = Vector3(ray_d[i][j].x, ray_d[i][j].y, -ray_d[i][j].z);
                double ray_d_n_dot = nrms[i][j].dot(ray_d_sc);
                Vector3 r = ray_d_sc - nrms[i][j] * ray_d_n_dot * 2.0;
                r.z -= 1.0;
                double m = 2 * r.norm();
                double x = r.x / m + 0.5;
                double y = r.y / m + 0.5;
                x = 1 - x; y = 1 - y;
                if(x < 0) x = 0; if (x > 1) x = 1;
                if(y < 0) y = 0; if (y > 1) y = 1;
                int x_ = (int)std::round(x * matcap_img.cols);
                int y_ = (int)std::round(y * matcap_img.rows);

                image.at<cv::Vec3b>(i, j)[0] = matcap_img.at<cv::Vec3b>(y_, x_)[0];
                image.at<cv::Vec3b>(i, j)[1] = matcap_img.at<cv::Vec3b>(y_, x_)[1];
                image.at<cv::Vec3b>(i, j)[2] = matcap_img.at<cv::Vec3b>(y_, x_)[2];
            }
            else{
                image.at<cv::Vec3b>(i, j)[0] = 255;
                image.at<cv::Vec3b>(i, j)[1] = 255;
                image.at<cv::Vec3b>(i, j)[2] = 255;
            }
        }
    }
}

void render(cv::Mat &image, Model &model, cv::Mat &matcap_img, bool logger){
    clock_t start = clock();
    if(logger){
        std::cout << "camera_o: "; camera_o.print();
        std::cout << "camera_t: "; camera_t.print();
    }
    int height = image.rows;
    int width = image.cols;

    std::vector<std::vector<Vector3>> ray_d(height, std::vector<Vector3>(width, Vector3(0,0,0)));
    decide_ray_direction(ray_d, width, height);

    std::vector<std::vector<Vector3>> nrms(height, std::vector<Vector3>(width, Vector3(0, 0, 0)));
    std::vector<std::vector<bool>> collided(height, std::vector<bool>(width, false));
    sphere_tracing(model, ray_d, nrms, collided, width, height);

    matcap_texture(image, matcap_img, ray_d, nrms, collided, width, height);

    clock_t end = clock();
    if(logger) std::cout << "time: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;
}
