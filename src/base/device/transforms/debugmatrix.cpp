#include <base/device/transforms/debugmatrix.h>

// glm::ivecs
std::ostream &operator<< (std::ostream &out, const glm::ivec2 &vec) {
    out << "{"
        << vec.x << "," << vec.y
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::ivec3 &vec) {
    out << "{"
        << vec.x << "," << vec.y << ","<< vec.z
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::ivec4 &vec) {
    out << "{"
        << vec.x << "," << vec.y << "," << vec.z << "," << vec.w
        << "}";
    return out;
}

// glm::ivecs
std::ostream &operator<< (std::ostream &out, const glm::uvec2 &vec) {
    out << "{"
        << vec.x << "," << vec.y
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::uvec3 &vec) {
    out << "{"
        << vec.x << "," << vec.y << ","<< vec.z
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::uvec4 &vec) {
    out << "{"
        << vec.x << "," << vec.y << "," << vec.z << "," << vec.w
        << "}";
    return out;
}

// glm::vecs
std::ostream &operator<< (std::ostream &out, const glm::vec2 &vec) {
    out << "{"
        << vec.x << "," << vec.y
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec) {
    out << "{"
        << vec.x << "," << vec.y << ","<< vec.z
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) {
    out << "{"
        << vec.x << "," << vec.y << "," << vec.z << "," << vec.w
        << "}";
    return out;
}

// glm::dvecs
std::ostream &operator<< (std::ostream &out, const glm::dvec2 &vec) {
    out << "{"
        << vec.x << "," << vec.y
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::dvec3 &vec) {
    out << "{"
        << vec.x << "," << vec.y << ","<< vec.z
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::dvec4 &vec) {
    out << "{"
        << vec.x << "," << vec.y << "," << vec.z << "," << vec.w
        << "}";
    return out;
}

// glm::mats
std::ostream &operator<< (std::ostream &out, const glm::mat4 &m) {
    out << "{"
        << m[0][0] << "," << m[0][1] << "," << m[0][2] << "," << m[0][3] << ";\n"
        << m[1][0] << "," << m[1][1] << "," << m[1][2] << "," << m[1][3] << ";\n"
        << m[2][0] << "," << m[2][1] << "," << m[2][2] << "," << m[2][3] << ";\n"
        << m[3][0] << "," << m[3][1] << "," << m[3][2] << "," << m[3][3] << ";\n"
        << "}";
    return out;
}
std::ostream &operator<< (std::ostream &out, const glm::dmat4 &m) {
    out << "{"
        << m[0][0] << "," << m[0][1] << "," << m[0][2] << "," << m[0][3] << ";\n"
        << m[1][0] << "," << m[1][1] << "," << m[1][2] << "," << m[1][3] << ";\n"
        << m[2][0] << "," << m[2][1] << "," << m[2][2] << "," << m[2][3] << ";\n"
        << m[3][0] << "," << m[3][1] << "," << m[3][2] << "," << m[3][3] << ";\n"
        << "}";
    return out;
}

