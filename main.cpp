#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include "Thread/ThreadPool.h"
#include "./cv_utils/cvutils.h"
#include "./constant.h"

using namespace cv;
using namespace std;


int main()
{
    //读取视频或摄像头
    stiching::ThreadPool pool(4);
    vector<VideoCapture> capture;
    vector<Mat> img_list;
    vector<Mat> cylinder_maps;
    vector<Mat> frameVec;

    pool.async([&]() {
        for (const auto& i : Constant::CAM_ID::cam_ID)
        {
            VideoCapture cap(i.data());
            capture.push_back(cap);
            cap.release();
        }
   
        for (auto i = 0; i < capture.size(); i++) {
            Mat frame;
            capture[i] >> frame;
            frameVec.push_back(frame);
        }
        });

    cylinder_maps = cylinder_projection_map(Constant::WIDTH,
        Constant::HEIGHT,Constant::FOCAL);

    //////////////////////////////设置capture的参数////////////////////////////////////////////
    for (auto& _cap : capture) {
        _cap.set(CAP_PROP_FPS, 10);
    }

    pool.async([&]() {
        for (const auto& frame : frameVec) {
            Mat cylinder;
            cv::remap(frame, cylinder, cylinder_maps[0],
                cylinder_maps[1], cv::INTER_LINEAR,
                BORDER_CONSTANT, Scalar(0, 0, 0));
            img_list.push_back(cylinder);
        }
        });

    pool.async([=]() {
        stitching(img_list);
        });

    return 0;
}
