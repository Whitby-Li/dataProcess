//
// Created by whitby on 4/13/24.
//

#include <iostream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/ccalib/omnidir.hpp>

using namespace std;

cv::Mat rectifyImage(const cv::Mat &image, const cv::Mat &K, const cv::Mat &distort) {
    double fx = K.at<double>(0, 0), fy = K.at<double>(1, 1), cx = K.at<double>(0, 2), cy = K.at<double>(1, 2);
    double k1 = distort.at<double>(0), k2 = distort.at<double>(1), k3 = distort.at<double>(2), k4 = distort.at<double>(3);

    int w = image.cols, h = image.rows;
    cv::Mat rect_image(h, w, CV_8UC1);
    for (int u = 0; u < w; u++) {
        for (int v = 0; v < h; v++) {
            double a = (u - cx) / fx, b = (v - cy) / fy;
            double r = sqrt(a * a + b * b);
            double theta = atan(r);

            double theta2 = theta * theta;
            double theta4 = theta2 * theta2;
            double theta6 = theta2 * theta4;
            double theta8 = theta4 * theta4;
            double theta_d = theta * (1 + k1 * theta2 + k2 * theta4 + k3 * theta6 + k4 * theta8);

            double distort_x = theta_d * a / r, distort_y = theta_d * b / r;
            int distort_u = cvFloor(fx * distort_x + cx), distort_v = cvFloor(fy * distort_y + cy);

            rect_image.at<char>(v, u) = image.at<char>(distort_v, distort_u);
        }
    }

    return rect_image;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <data_folder>" << endl;
        return 1;
    }

    // 1. set camera matrix
    double fx = 190.978477, fy = 190.973307, cx = 254.931706, cy = 256.897442;
    double k1 = 0.003482389402, k2 = 0.000715034845, k3 = -0.002053236141, k4 = 0.000202936736;

    cv::Mat K = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    cv::Mat distort = (cv::Mat_<double>(4, 1) << k1, k2, k3, k4);

    /*double fx = 533.340727445877, fy = 533.2556495307942, cx = 254.64689387916482, cy = 256.4835490935692;
    double k1 = -0.05972430882700243, k2 = 0.17468739202093328, p1 = 0.000737218969875311, p2 = 0.000574074894976456;
    double xi = 1.792187901303534;

    cv::Mat K = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
    cv::Mat distort = (cv::Mat_<double>(4, 1) << k1, k2, p1, p2);*/

    int newW = 640, newH = 640;
    cv::Mat newK = (cv::Mat_<double>(3, 3) << 150, 0, newW / 2, 0, 150, newH / 2, 0, 0, 1);

    // 2. rectify images
    const string dataFolder = argv[1];
    const string imgFolder1 = dataFolder + "/cam0", imgFolder2 = dataFolder + "/cam1";
    const string outFolder1 = dataFolder + "/rect_cam0", outFolder2 = dataFolder + "/rect_cam1";

    if (!filesystem::exists(outFolder1)) {
        filesystem::create_directories(outFolder1 + "/data");
        cout << "create " << outFolder1 << endl;
    }

    if (!filesystem::exists(outFolder2)) {
        filesystem::create_directories(outFolder2 + "/data");
        cout << "create " << outFolder2 << endl;
    }

    for (int i = 0; ; i++) {
        if (i % 100 == 0) cout << "iter: " << i << endl;
        string img_name = cv::format("/data/%08d.png", i);
        cv::Mat img1 = cv::imread(imgFolder1 + img_name, cv::IMREAD_GRAYSCALE);
        cv::Mat img2 = cv::imread(imgFolder2 + img_name, cv::IMREAD_GRAYSCALE);

        if (img1.empty() || img2.empty()) {
            cout << "can't read image at " << img_name << endl;
            break;
        }

        cv::Mat rect_img1, rect_img2;
        cv::fisheye::undistortImage(img1, rect_img1, K, distort, newK, {newW, newH});
        cv::fisheye::undistortImage(img2, rect_img2, K, distort, newK, {newW, newH});

        cv::imwrite(outFolder1 + img_name, rect_img1);
        cv::imwrite(outFolder2 + img_name, rect_img2);
    }

    filesystem::copy_file(imgFolder1 + "/times.txt", outFolder1 + "/times.txt");
    filesystem::copy_file(imgFolder2 + "/times.txt", outFolder2 + "/times.txt");

    return 0;
}