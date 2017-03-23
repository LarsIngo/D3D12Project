#pragma once

#include <glm/glm.hpp>

struct Particle
{
    glm::vec4 position = glm::vec4(0.f, 0.f, 0.f, 0.f);
    glm::vec4 velocity = glm::vec4(0.f, 0.f, 0.f, 0.f);
    glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    glm::vec4 scale = glm::vec4(1.f, 1.f, 0.f, 0.f);
};
