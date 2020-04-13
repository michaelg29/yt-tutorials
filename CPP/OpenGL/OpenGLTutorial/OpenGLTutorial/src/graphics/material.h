#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;

    static Material emerald;
    static Material jade;
    static Material obsidian;
    static Material pearl;
    static Material ruby;
    static Material turquoise;
    static Material brass;
    static Material bronze;
    static Material chrome;
    static Material copper;
    static Material gold;
    static Material silver;
    static Material black_plastic;
    static Material cyan_plastic;
    static Material green_plastic;
    static Material red_plastic;
    static Material white_plastic;
    static Material yellow_plastic;
    static Material black_rubber;
    static Material cyan_rubber;
    static Material green_rubber;
    static Material red_rubber;
    static Material white_rubber;
    static Material yellow_rubber;

    static Material mix(Material m1, Material m2, float mix = 0.5f);
};

#endif