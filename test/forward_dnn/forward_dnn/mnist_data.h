#pragma once
#include <string>
#include <vector>

class Mnist
{
public:
    const static int imgWidth_ = 28;
    const static int imgHeight_ = 28;
    struct TestImg
    {
        char label;
        char data[imgHeight_][imgWidth_];
    };

    struct Prefix
    {
        int magic;
        int num;
        int width;
        int height;
    };

public:
    Mnist();
    ~Mnist();

    int getTestData(int index, char** data, char* label);

private:
    std::string imageFileName_ = "t10k-images.idx3-ubyte";
    std::string labelFileName_ = "t10k-labels.idx1-ubyte";
    std::vector<TestImg*> testImages_;
};
