//
// Created by daily on 01-01-24.
//
#include "scene.hpp"

Scene::Scene()
{
//eTriangleList

//    for(float x = -1.0f; x < 1.0f; x += 0.2f)
//    {
//        for(float y = -1.0f; y < 1.0f; y += 0.2f)
//        {
//            triangle_positions_.push_back(glm::vec3(x, y, 0.0f));
//        }
//    }

//eTriangleStrip
    triangle_positions_.emplace_back(-0.5f, 0.0f, 0.0f);
    triangle_positions_.emplace_back(0.5f, 0.0f, 0.0f);
}