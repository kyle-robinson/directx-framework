#pragma once
#ifndef OBJECTINDICES_H
#define OBJECTINDICES_H

typedef unsigned short WORD;

namespace IDX
{
    WORD indicesLightCube[] =
    {
        23, 22, 20,
        22, 21, 20,
        19, 18, 16,
        18, 17, 16,
        15, 14, 12,
        14, 13, 12,
        11, 10, 8,
        10, 9, 8,
        7, 6, 4,
        6, 5, 4,
        3, 2, 0,
        2, 1, 0

        //0, 1, 2,
        //0, 2, 3,
        //4, 5, 6,
        //4, 6, 7,
        //8, 9, 10,
        //8, 10, 11,
        //12, 13, 14,
        //12, 14, 15,
        //16, 17, 18,
        //16, 18, 19,
        //20, 21, 22,
        //20, 22, 23,
    };

    // cube
    WORD indicesCube[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 0, 6,    // side 2
        6, 0, 2,
        7, 5, 6,    // side 3
        6, 5, 4,
        3, 1, 7,    // side 4
        7, 1, 5,
        4, 5, 0,    // side 5
        0, 5, 1,
        3, 7, 2,    // side 6
        2, 7, 6
    };

    // pyramid
    WORD indicesPyramid[] =
    {
        0, 2, 1,    // base
        1, 2, 3,
        0, 1, 4,    // sides
        1, 3, 4,
        3, 2, 4,
        2, 0, 4,
    };

    // quad
    WORD indicesQuad[] =
    {
        0, 1, 3,
        1, 2, 3
    };
}

#endif