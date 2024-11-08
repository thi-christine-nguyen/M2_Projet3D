#ifndef CAMERA_HELPER_HPP__
#define CAMERA_HELPER_HPP__
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#define M_PI       3.14159265358979323846f   // pi

static glm::vec3 VEC_ZERO{ 0.f,0.f,0.f };
static glm::vec3 VEC_UP{ 0.f,1.f,0.f };
static glm::vec3 VEC_FRONT{ 0.f,0.f,1.f };
static glm::vec3 VEC_RIGHT{ 1.f,0.f,0.f };

enum class InterpolationType
{
    Linear,
    Cosine,
    Exponential,
    Logarithmic
};

class Camera_Helper
{
public: 
	static glm::vec3 quatToEuler(glm::quat _quat)
    {
        // Opengl quat to euler function give yaw betweel -90 and 90
        // If you want correct pitch and yaw you can use this 
        // Src http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
        // But roll will be bewteen -90 and 90 here

        glm::vec3 eulerAngles;

        float test = _quat.x * _quat.y + _quat.z * _quat.w;

        if (test > 0.499f) { // singularity at north pole
            eulerAngles.y = 2.f * atan2(_quat.x, _quat.w);
            eulerAngles.z = M_PI / 2.f;
            eulerAngles.x = 0.f;
            return eulerAngles;
        }
        if (test < -0.499f) { // singularity at south pole
            eulerAngles.y = -2.f * atan2(_quat.x, _quat.w);
            eulerAngles.z = -M_PI / 2.f;
            eulerAngles.x = 0.f;
            return eulerAngles;
        }
        float sqx = _quat.x * _quat.x;
        float sqy = _quat.y * _quat.y;
        float sqz = _quat.z * _quat.z;


        eulerAngles.y = atan2(2.f * _quat.y * _quat.w - 2.f * _quat.x * _quat.z, 1.f - 2.f * sqy - 2.f * sqz);
        eulerAngles.z = asin(2.f * test);
        eulerAngles.x = atan2(2.f * _quat.x * _quat.w - 2.f * _quat.y * _quat.z, 1.f - 2.f * sqx - 2.f * sqz);

        return eulerAngles;
    }
    
	static void computeFinalView(glm::mat4& _outProjectionMatrix, glm::mat4& _outviewMatrix, glm::vec3& _position, glm::quat _rotation, float _fovDegree)
    {
        // Projection matrix : FOV, 4:3 ratio, display range : 0.1 unit <-> 100 units
        _outProjectionMatrix = glm::perspective(glm::radians(_fovDegree), 4.0f / 3.0f, 0.1f, 200.0f);

        const glm::vec3 front = normalize(_rotation* VEC_FRONT);
        const glm::vec3 up = normalize(_rotation * VEC_UP);

        // Camera matrix
        _outviewMatrix = glm::lookAt(_position, _position + front, up);
    }

    static float ClipAngle180(float _angle)
    {
        while (_angle >= 180.0f)
            _angle -= 360.0f;
        while (_angle < -180.0f)
            _angle += 360.0f;

        return fmod(_angle, 180);
    }

    static glm::vec3 ProjectVectorOnPlane(const glm::vec3& _vectorToProject, const glm::vec3& _planeNormal)
    {
        return _vectorToProject - glm::dot(_vectorToProject, _planeNormal) * _planeNormal;
    }

    static float clipAngle(float angle, float value)
    {
        
    // Limiter l'angle entre -value et value
        angle = glm::clamp(angle, -value, value);
        return glm::radians(angle);
    }

    static float Interpolation(float _ratio, InterpolationType _type)
    {
        switch (_type)
        {
        case InterpolationType::Linear:
            return _ratio; // Interpolation linéaire, pas de modification
        case InterpolationType::Cosine:
            return (1 - cos(_ratio * M_PI)) / 2; // Interpolation cosine
        case InterpolationType::Exponential:
            return exp(_ratio); // Interpolation exponentielle
        case InterpolationType::Logarithmic:
            return log(_ratio + 1); // Interpolation logarithmique
        default:
            return _ratio; // Par défaut, pas de modification
        }
    }
};
#endif