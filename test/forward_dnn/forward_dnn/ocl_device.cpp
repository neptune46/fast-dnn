#include "pch.h"
#include "ocl_device.h"

#include <vector>
#include <stdio.h>

#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f

OclEngine::OclEngine()
{
}

OclEngine::~OclEngine()
{
}

bool OclEngine::checkPlatform(cl_platform_id platform, const char* preferredPlatform)
{
    size_t stringLength = 0;
    cl_int err = CL_SUCCESS;
    bool match = false;

    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &stringLength);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetPlatformInfo() to get CL_PLATFORM_NAME length returned '%d'.\n", err);
        return false;
    }

    std::vector<char> platformName(stringLength);
    err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, stringLength, &platformName[0], NULL);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetplatform_ids() to get CL_PLATFORM_NAME returned %d.\n", err);
        return false;
    }

    if (strstr(&platformName[0], preferredPlatform) != 0)
    {
        match = true;
    }

    return match;
}

cl_platform_id OclEngine::findPlatform(cl_device_type deviceType, const char* preferredPlatform)
{
    cl_uint numPlatforms = 0;
    cl_int ret = CL_SUCCESS;

    ret = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (CL_SUCCESS != ret)
    {
        printf("ERROR: failed to call clGetPlatformIDs\n");
        return nullptr;
    }

    if (numPlatforms <= 0)
    {
        printf("ERROR: no platform found\n");
        return nullptr;
    }

    std::vector<cl_platform_id> platforms(numPlatforms);
    ret = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
    if (CL_SUCCESS != ret)
    {
        printf("ERROR: failed to call clGetPlatformIDs\n");
        return nullptr;
    }

    for (cl_uint i = 0; i < numPlatforms; i++)
    {
        bool match = true;
        cl_uint numDevices = 0;

        match = checkPlatform(platforms[i], preferredPlatform);

        if (match)
        {
            ret = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);
            if (CL_SUCCESS != ret)
            {
                printf("ERROR: clGetDeviceIDs() returned %d.\n", ret);
            }

            if (0 != numDevices)
            {
                return platforms[i];
            }
        }
    }

    return nullptr;
}

int OclEngine::getVersions()
{
    cl_int err = CL_SUCCESS;

    size_t stringLength = 0;
    err = clGetPlatformInfo(platform_, CL_PLATFORM_VERSION, 0, NULL, &stringLength);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetPlatformInfo() to get CL_PLATFORM_VERSION length returned '%d'.\n", err);
        return err;
    }

    std::vector<char> platformVersion(stringLength);
    err = clGetPlatformInfo(platform_, CL_PLATFORM_VERSION, stringLength, &platformVersion[0], NULL);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetplatform_ids() to get CL_PLATFORM_VERSION returned %d.\n", err);
        return err;
    }

    if (strstr(&platformVersion[0], "OpenCL 2.0") != NULL)
    {
        platformVersion_ = OPENCL_VERSION_2_0;
    }

    err = clGetDeviceInfo(device_, CL_DEVICE_VERSION, 0, NULL, &stringLength);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION length returned '%d'.\n", err);
        return err;
    }

    std::vector<char> deviceVersion(stringLength);
    err = clGetDeviceInfo(device_, CL_DEVICE_VERSION, stringLength, &deviceVersion[0], NULL);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION returned %d.\n", err);
        return err;
    }

    if (strstr(&deviceVersion[0], "OpenCL 2.0") != NULL)
    {
        deviceVersion_ = OPENCL_VERSION_2_0;
    }

    err = clGetDeviceInfo(device_, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &stringLength);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION length returned '%d'.\n", err);
        return err;
    }

    std::vector<char> compilerVersion(stringLength);
    err = clGetDeviceInfo(device_, CL_DEVICE_OPENCL_C_VERSION, stringLength, &compilerVersion[0], NULL);
    if (CL_SUCCESS != err)
    {
        printf("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION returned %d.\n", err);
        return err;
    }
    else if (strstr(&compilerVersion[0], "OpenCL C 2.0") != NULL)
    {
        compilerVersion_ = OPENCL_VERSION_2_0;
    }

    return err;
}

int OclEngine::create(cl_device_type deviceType, const char* platformName)
{
    cl_int ret = CL_SUCCESS;

    platform_ = findPlatform(deviceType, platformName);
    if (platform_ == nullptr)
    {
        printf("ERROR: Failed to find OpenCL platform.\n");
        return -1;
    }

    // Create context with device of specified type.
    cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_, 0 };
    context_ = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &ret);
    if ((CL_SUCCESS != ret) || (nullptr == context_))
    {
        printf("Couldn't create a context, clCreateContextFromType() returned %d.\n", ret);
        return ret;
    }

    // Query for OpenCL device which was used for context creation
    ret = clGetContextInfo(context_, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device_, NULL);
    if (CL_SUCCESS != ret)
    {
        printf("Error: clGetContextInfo() to get list of devices returned %d.\n", ret);
        return ret;
    }

    getVersions();

    if (OPENCL_VERSION_2_0 != deviceVersion_)
    {
        printf("ERROR: device doesn't support OpenCL 2.0\n");
        return -1;
    }

    const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
    commandQueue_ = clCreateCommandQueueWithProperties(context_, device_, properties, &ret);
    if (CL_SUCCESS != ret)
    {
        printf("Error: clCreateCommandQueue() returned %d.\n", ret);
        return ret;
    }

    return 0;
}

