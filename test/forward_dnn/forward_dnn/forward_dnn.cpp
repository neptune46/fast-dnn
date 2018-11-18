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

struct PassRate
{
    int passCount;
    int totalCount;
};

char findMaxIndex(float* prob, int length)
{
    int index = 0;
    float max = 0.0;
    for (int i=0; i<length; i++)
    {
        if (*(prob+i) > max)
        {
            index = i;
            max = *(prob + i);
        }
    }
    return index;
}

int opencv_forward()
{
    PassRate stat = {};
    Net net = readNet("mnist.pb", "", "tensorflow");
    net.setPreferableBackend(0);
    net.setPreferableTarget(1);

    Mnist mnist;
    int index = 0;
    char *data, label, result;
    while (mnist.getTestData(index++, &data, &label) == 0)
    {
        Mat img(28, 28, CV_8U, data);
        Mat src(28, 28, CV_32F);
        img.convertTo(src, CV_32F);
        src = src / 255.0;

        int sz[] = { 1, 1, 28, 28 };
        Mat blob(4, sz, CV_32F, src.data);
        net.setInput(blob);
        Mat prob = net.forward();

        result = findMaxIndex((float*)prob.data, prob.cols);
        if (result == label)
        {
            stat.passCount++;
        }
        stat.totalCount++;

        index++;
    }
    double rate = (double)stat.passCount / (double)stat.totalCount;
    cout << "pass rate = " << rate << endl;

    return 0;
}

int main()
{
    opencv_forward();

    return 0;
}
