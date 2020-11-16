#pragma once
#ifndef MODELDATA_H
#define MODELDATA_H

#include "nlohmann/json.hpp"
using json = nlohmann::json;

struct Drawable
{
    std::string modelName;
    std::string fileName;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
};
std::vector<Drawable> drawables;

#endif