#pragma once
#include <string>

struct DeviceInfo 
{
    std::string instanceID;
    std::string className;
    std::string manufacturer;
    std::string devDescription;
};

enum class ResultCommand
{
    Success = 0,
    Error = 1,
};