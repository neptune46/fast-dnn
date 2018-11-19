// forward_dnn.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <math.h>

#include "model_data.h"
#include "mnist_data.h"
#include "test_data.h"
#include "perf_util.h"

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

PerfUtil perftool;

struct PassRate
{
    int passCount;
    int totalCount;
};

template<typename T>
char findMaxIndex(T* prob, int length)
{
    int index = 0;
    T max = 0.0;
    for (int i = 0; i < length; i++)
    {
        if (*(prob + i) > max)
        {
            index = i;
            max = *(prob + i);
        }
    }
    return index;
}

int cpuref_image(unsigned char* img)
{
    float result1[128];
    for (int y = 0; y < 128; y++)
    {
        result1[y] = dense_bias[y];
        for (int x = 0; x < 784; x++)
        {
            result1[y] += ((float)img[x] / (float)255.0) * dense_kernel[x][y];
        }
        result1[y] = (result1[y] < 0) ? 0 : result1[y];
    }

    float result2[10];
    double exp2[10], sum2 = 0.0;
    for (int y = 0; y < 10; y++)
    {
        result2[y] = dense_1_bias[y];
        for (int x = 0; x < 128; x++)
        {
            result2[y] += result1[x] * dense_1_kernel[x][y];
        }
        exp2[y] = exp(result2[y]);
        sum2 += exp2[y];
    }

    double output[10];
    for (int i = 0; i < 10; i++)
    {
        output[i] = exp2[i] / sum2;
    }

    return findMaxIndex<double>(output, 10);
}

int forward_cpuref(Mnist* mnist)
{
    PassRate stat = {};
    int index = 0;
    char *data, label, result;

    cout << "start CPU ref inference: " << endl;
    while (mnist->getTestData(index++, &data, &label) == 0)
    {
        perftool.startTick("cpuref-forward");
        result = cpuref_image((unsigned char*)data);
        perftool.stopTick("cpuref-forward");

        if (result == label)
        {
            stat.passCount++;
        }
        stat.totalCount++;
    }
    cout << endl << "CPU ref done with " << stat.totalCount << " images." << endl;

    double rate = (double)stat.passCount / (double)stat.totalCount;
    cout << "pass rate = " << rate << endl;

    return 0;
}

int forward_opencv(Mnist* mnist)
{
    PassRate stat = {};
    Net net = readNet("mnist.pb", "", "tensorflow");
    net.setPreferableBackend(0);
    net.setPreferableTarget(1);

    int index = 0;
    char *data, label, result;
    cout << "start OpenCV inference: " << endl;
    while (mnist->getTestData(index++, &data, &label) == 0)
    {
        Mat img(28, 28, CV_8U, data);
        Mat src(28, 28, CV_32F);
        img.convertTo(src, CV_32F);
        src = src / 255.0;

        int sz[] = { 1, 1, 28, 28 };
        Mat blob(4, sz, CV_32F, src.data);
        net.setInput(blob);

        perftool.startTick("opencv-forward");
        Mat prob = net.forward();
        perftool.stopTick("opencv-forward");

        result = findMaxIndex<float>((float*)prob.data, prob.cols);
        if (result == label)
        {
            stat.passCount++;
        }
        stat.totalCount++;
    }
    cout << endl << "OpenCV done with " << stat.totalCount << " images." << endl;

    double rate = (double)stat.passCount / (double)stat.totalCount;
    cout << "pass rate = " << rate << endl;

    return 0;
}

int main()
{
    Mnist mnist;
    //mnist.mergeImage();

    forward_opencv(&mnist);

    forward_cpuref(&mnist);

    return 0;
}
