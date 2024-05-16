//
// Created by whitby on 5/4/24.
//

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Imu.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <iomanip>
#include <filesystem>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <bag_file.bag>" << endl;
        return 1;
    }

    // 1. get bag file path and its parent directory
    const string bagPath = argv[1];
    string dataFolder;

    size_t lastSlashPos = bagPath.find_last_of('/');
    if (lastSlashPos != string::npos) {
        dataFolder = bagPath.substr(0, lastSlashPos);
    } else {
        cerr << "fail to get parent directory" << endl;
        return 1;
    }

    cout << "start to parse " << bagPath << endl;

    // 2. create image folder
    const string leftImgFolder = dataFolder + "/cam0/data";
    const string rightImgFolder = dataFolder + "/cam1/data";

    if (!filesystem::exists(leftImgFolder)) {
        filesystem::create_directories(leftImgFolder);
        cout << "create directory " << leftImgFolder << endl;
    }

    if (!filesystem::exists(rightImgFolder)) {
        filesystem::create_directories(rightImgFolder);
        cout << "create directory " << rightImgFolder << endl;
    }

    // 2. open bag
    rosbag::Bag bag;
    try {
        bag.open(bagPath, rosbag::bagmode::Read);
    } catch (rosbag::BagException &e) {
        cerr << "Error opening bag: " << e.what() << endl;
        return 1;
    }

    // 3. set filestream
    ofstream camOut1(dataFolder + "/cam0/times.txt"), camOut2(dataFolder + "/cam1/times.txt");
    ofstream imuOut(dataFolder + "/imu.txt");

    camOut1 << fixed << setprecision(6);
    camOut2 << fixed << setprecision(6);
    imuOut << fixed << setprecision(6);

    // 4. read bag
    int idx1 = 0, idx2 = 0;
    rosbag::View view(bag);
    size_t num_messages = view.size();
    size_t processed_messages = 0;
    for (rosbag::MessageInstance const m: view) {
        const string &topic = m.getTopic();

        if (topic == "/cam0/image_raw") {
            sensor_msgs::Image::ConstPtr image_msg = m.instantiate<sensor_msgs::Image>();
            cv_bridge::CvImagePtr cv_ptr;
            try {
                cv_ptr = cv_bridge::toCvCopy(image_msg, sensor_msgs::image_encodings::BGR8); // 假设输出图像格式为BGR8
            } catch (cv_bridge::Exception& e) {
                std::cerr << "cv_bridge exception: " << e.what() << std::endl;
                continue;
            }

            const string filename = leftImgFolder + cv::format("/%08d.png", idx1);
            cv::Mat grayImg;
            cv::cvtColor(cv_ptr->image, grayImg, CV_BGR2GRAY);
            cv::imwrite(filename, grayImg);
            camOut1 << image_msg->header.stamp.toSec() << endl;
            idx1++;
        } else if (topic == "/cam1/image_raw") {
            sensor_msgs::Image::ConstPtr image_msg = m.instantiate<sensor_msgs::Image>();
            cv_bridge::CvImagePtr cv_ptr;
            try {
                cv_ptr = cv_bridge::toCvCopy(image_msg, sensor_msgs::image_encodings::BGR8); // 假设输出图像格式为BGR8
            } catch (cv_bridge::Exception& e) {
                std::cerr << "cv_bridge exception: " << e.what() << std::endl;
                continue;
            }

            const string filename = rightImgFolder + cv::format("/%08d.png", idx2);
            cv::Mat grayImg;
            cv::cvtColor(cv_ptr->image, grayImg, CV_BGR2GRAY);
            cv::imwrite(filename, grayImg);
            camOut2 << image_msg->header.stamp.toSec() << endl;
            idx2++;
        } else if (topic == "/imu0") {
            sensor_msgs::ImuConstPtr imu_msg = m.instantiate<sensor_msgs::Imu>();
            const auto &angularVelo = imu_msg->angular_velocity;
            const auto &linearAcc = imu_msg->linear_acceleration;

            imuOut << imu_msg->header.stamp << " " << angularVelo.x << " " << angularVelo.y << " " << angularVelo.z
                   << " "
                   << linearAcc.x << " " << linearAcc.y << " " << linearAcc.z << endl;
        }

        processed_messages++;
        if (processed_messages % 1000 == 0)
            cout << dataFolder << ": " << processed_messages << " / " << num_messages << endl;
    }

    // 5. close filestream and bag
    camOut1.close();
    camOut2.close();
    imuOut.close();
    bag.close();

    return 0;
}