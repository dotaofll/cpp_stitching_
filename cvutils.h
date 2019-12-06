#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

using vecMat = vector<Mat>;

    vecMat cylinder_projection_map(double width, double height, double focal)
{

    Mat_<float> map_x = Mat::zeros(height, width, CV_16SC1);
    Mat_<float> map_y = Mat::zeros(height, width, CV_16SC1);
    ////////////////////////////////////////////////////
    for (int c_y = -int(height / 2); c_y < int(height / 2); c_y++)
    {
        for (int c_x = -int(width / 2); c_x < int(width / 2); c_x++)
        {
            long double x = focal * tan(c_x / focal);
            long double y = c_y * sqrt(pow(x, 2) + pow(focal, 2)) / focal + height / 2.0;
            x += width / 2.0;
            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                int location_x = c_y + int(height / 2);
                int location_y = c_x + int(width / 2);
                map_x.at<float>(location_x, location_y) = x;
                map_y.at<float>(location_x, location_y) = y;
            }
        }
    }
    vecMat maps{map_x, map_y};
    return maps;
}

//优化缝合线
Mat optimizeSeam(Mat &img1, Mat &img2, Mat &dst)
{
    auto start{ 600 };                          //开始拼接处
    double process_width = img1.cols - start; //处理的宽度
    int rows = dst.rows;                      //高度
    int cols = img1.cols;
    double alpha{1};
    for (int i = 0; i < rows; i++)
    {
        uchar *p = img1.ptr<uchar>(i);
        uchar *t = img2.ptr<uchar>(i);
        uchar *d = dst.ptr<uchar>(i);
        for (int j = start + process_width / 2; j < cols + process_width / 2; ++j)
        {
            if (t[j * 3] == 0 && t[j * 3 + 1] == 0 && t[j * 3 + 2] == 0)
            { //如果是黑色的区域就设置　alpha为０
                alpha = 1;
            }
            else
            {
                alpha = (process_width - (j - start)) / process_width;
            }
            d[j * 3] = p[j * 3] * alpha + t[j * 3] * (1 - alpha);
            d[j * 3 + 1] = p[j * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
            d[j * 3 + 2] = p[j * 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);
        }
    }
}

//拼接
Mat stitching(vecMat img_list)
{

    Mat dispimg, combine1, combine2;
    //4幅图像合并成一幅图像显示
    //    cv::resize(img_list[0], img_list[0], cv::Size(640, 480), 0, 0, CV_INTER_LINEAR);
    //    cv::resize(img_list[1], img_list[1], cv::Size(640, 480), 0, 0, CV_INTER_LINEAR);
    //    cv::resize(img_list[2], img_list[2], cv::Size(640, 480), 0, 0, CV_INTER_LINEAR);
    //    cv::resize(img_list[3], img_list[3], cv::Size(640, 480), 0, 0, CV_INTER_LINEAR);

    //    combine1.create(Size(img_list[0].cols+img_list[1].cols+img_list[2].cols+img_list[3].cols, max(img_list[0].rows,img_list[0].rows)),CV_8UC3);
    //    combine1.setTo(0);
    //    img_list[1].copyTo(combine1(cv::Rect(600,0,img_list[0].cols, img_list[0].rows)));
    //    img_list[0].copyTo(combine1(cv::Rect(0,0,img_list[0].cols, img_list[0].rows)));
    //    optimizeSeam(img_list[0], img_list[1],combine1);
    //    img_list[2].copyTo(combine1(cv::Rect(1200,0,img_list[0].cols, img_list[0].rows)));
    //    img_list[3].copyTo(combine1(cv::Rect(1800,0,img_list[0].cols, img_list[0].rows)));

    //    cv::Mat H1 = (cv::Mat_<double>(3,3) <<
    //                                       1, 0, 0,
    //                                       0, 1, 0,
    //                                       0, 0, 1);
    //    cv::Mat H1 = (cv::Mat_<double>(3,3) <<
    //                                       1, 0, 600,
    //                                       0, 1, 0,
    //                                       0, 0, 1);
    //    cv::warpPerspective(img_list[1], combine1, H1 , cv::Size(1500, 500));
    //    img_list[0].copyTo(combine1())
    //    cv::warpPerspective(img_list[1], combine1, H2 , cv::Size(1500, 500));

    //水平拼接
    //    hconcat(img_list[0], img_list[1], combine1);

    hconcat(img_list[0], img_list[1], combine1);
    hconcat(img_list[2], img_list[3], combine2);
    hconcat(combine1, combine2, dispimg);
    //融合

    //    namedWindow("Combine", CV_WINDOW_AUTOSIZE);
    imshow("Combine", dispimg);
    cv::waitKey(1);
}
