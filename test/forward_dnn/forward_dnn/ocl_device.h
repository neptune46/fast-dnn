#pragma once
#include "CL\cl.h"

class OclEngine
{
public:
    OclEngine();
    ~OclEngine();

    int create(cl_device_type deviceType, const char* platformName);
    void destroy();

private:
    bool checkPlatform(cl_platform_id platform, const char* preferredPlatform);
    cl_platform_id findPlatform(cl_device_type deviceType, const char* preferredPlatform);
    int getVersions();

private:
    cl_context       context_;
    cl_platform_id   platform_;
    cl_device_id     device_;
    cl_command_queue commandQueue_;
    cl_program       program_;
    cl_kernel        kernel_;
    float            platformVersion_;
    float            deviceVersion_;
    float            compilerVersion_;
};


