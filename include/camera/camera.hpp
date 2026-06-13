#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float FOV        =  45.0f;



struct Plane
{
	glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
	float distance = 0.f;        // Distance with origin

	Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		distance(glm::dot(normal, p1))
	{}

	float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane leftFace;
    Plane rightFace;

    Plane nearFace;
    Plane farFace;
};

// An abstract camera class that processes input and calculates the corresponding orientation vectors and matrices for use in OpenGL
class Camera
{
    public:
        // camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        glm::quat Orientation;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float fov;

        Frustum camFrustum;

        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, int scr_width = 1240, int scr_height = 720) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), fov(FOV)
        {
            Position = position;
            WorldUp = up;
            Orientation = glm::normalize(glm::angleAxis(glm::radians(yaw), WorldUp) * glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)));
            updateCameraVectors(scr_width, scr_height);
        }
        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, int scr_width, int scr_height) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), fov(FOV)
        {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Orientation = glm::normalize(glm::angleAxis(glm::radians(yaw), WorldUp) * glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)));
            updateCameraVectors(scr_width, scr_height);
        }
        glm::mat4 GetViewMatrix();
        void PanCamera(float X, float Y, float deltaTime, int scr_width, int scr_height);
        void RotateCamera(int scr_width, int scr_height, float xoffset, float yoffset, GLboolean constrainPitch = true);
        void MoveCameraForward(float yoffset, int scr_width, int scr_height);
        Frustum createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar);

    private:

        void updateCameraVectors(int scr_width, int scr_height);
};
#endif