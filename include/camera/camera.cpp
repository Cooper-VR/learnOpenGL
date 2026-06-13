#include <camera/camera.hpp>

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::PanCamera(float X, float Y, float deltaTime, int scr_width, int scr_height)
{
    float velocity = MovementSpeed * deltaTime;
    if (X != 0)
        Position += Right * velocity * X;
    if (Y != 0)
        Position -= Up * velocity * Y;

    updateCameraVectors(scr_width, scr_height);
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::RotateCamera(int scr_width, int scr_height, float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= -MouseSensitivity;
    yoffset *= MouseSensitivity;

    glm::quat yawRotation = glm::angleAxis(glm::radians(xoffset), glm::normalize(WorldUp));
    glm::quat yawedOrientation = glm::normalize(yawRotation * Orientation);
    glm::vec3 pitchAxis = glm::normalize(glm::rotate(yawedOrientation, glm::vec3(1.0f, 0.0f, 0.0f)));
    glm::quat pitchRotation = glm::angleAxis(glm::radians(yoffset), pitchAxis);

    glm::quat candidateOrientation = glm::normalize(pitchRotation * yawedOrientation);

    if (constrainPitch)
    {
        glm::vec3 candidateFront = glm::rotate(candidateOrientation, glm::vec3(0.0f, 0.0f, -1.0f));
        if (glm::abs(glm::dot(glm::normalize(candidateFront), glm::normalize(WorldUp))) > 0.99f)
        {
            candidateOrientation = yawedOrientation;
        }
    }

    Orientation = candidateOrientation;

    // update Front, Right and Up Vectors using the updated orientation
    updateCameraVectors(scr_width, scr_height);
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::MoveCameraForward(float yoffset, int scr_width, int scr_height)
{
    //Zoom -= (float)yoffset;
    Camera::Position += Front * (yoffset * 0.2f);

    updateCameraVectors(scr_width, scr_height);
}

// calculates the direction vectors from the Camera's updated orientation
void Camera::updateCameraVectors(int scr_width, int scr_height)
{
    glm::mat3 rotationMatrix = glm::mat3_cast(Orientation);

    Front = glm::normalize(rotationMatrix * glm::vec3(0.0f, 0.0f, -1.0f));
    Right = glm::normalize(rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f));
    Up = glm::normalize(glm::cross(Right, Front));

    camFrustum = createFrustumFromCamera((float)scr_width / (float)scr_height, glm::radians(fov), 0.001f, 1000.0f);
}


Frustum Camera::createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar)
{
	Frustum frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = zFar * Front;

	frustum.nearFace = { Position + zNear * Front, Front };
	frustum.farFace = { Position + frontMultFar, -Front };
	frustum.rightFace = { Position, glm::cross(frontMultFar - Right * halfHSide, Up) };
	frustum.leftFace = { Position, glm::cross(Up, frontMultFar + Right * halfHSide) };
	frustum.topFace = { Position, glm::cross(Right, frontMultFar - Up * halfVSide) };
	frustum.bottomFace = { Position, glm::cross(frontMultFar + Up * halfVSide, Right) };
	return frustum;
}