#pragma once
#ifndef MODELDATA_H
#define MODELDATA_H

#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct Drawable
{
    std::string modelName;
    std::string fileName;
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float scaleX, scaleY, scaleZ;
};
std::vector<Drawable> drawables;

#endif