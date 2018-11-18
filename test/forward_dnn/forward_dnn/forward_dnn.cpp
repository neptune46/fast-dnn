// forward_dnn.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "model_data.h"
#include "mnist_data.h"
#include "test_data.h"

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

int opencv_forward()
{
    Net net = readNet("mnist.pb", "", "tensorflow");
    net.setPreferableBackend(0);
    net.setPreferableTarget(1);

    Mat img(28, 28, CV_8U, test_img);
    Mat src(28, 28, CV_32F);
    img.convertTo(src, CV_32F);
    src = src / 255.0;

    int sz[] = { 1, 1, 28, 28};
    Mat blob(4, sz, CV_32F, src.data);
    net.setInput(blob);
    Mat prob = net.forward();

    cout << "result: " <<prob << endl;

    return 0;
}

int main()
{
    opencv_forward();

    return 0;
}
