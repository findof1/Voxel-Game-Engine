#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "math.h"

enum CameraMovement
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

struct Frustum
{
  glm::vec4 planes[6]; // left, right, top, bottom, near, far
};

class Camera
{
public:
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  float Yaw;
  float Pitch;

  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  Camera(const Camera &other)
  {
    Position = other.Position;
    Front = other.Front;
    Up = other.Up;
    Right = other.Right;
    WorldUp = other.WorldUp;

    Yaw = other.Yaw;
    Pitch = other.Pitch;

    MovementSpeed = other.MovementSpeed;
    MouseSensitivity = other.MouseSensitivity;
    Zoom = other.Zoom;

    // Update vectors just to be safe
    updateCameraVectors();
  }

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         float yaw = -90.0f,
         float pitch = 0.0f)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(12.5f),
        MouseSensitivity(0.1f),
        Zoom(75.0f)
  {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix() const
  {
    return glm::lookAt(Position, Position + Front, Up);
  }

  glm::mat4 getProjectionMatrix(float aspectRatio) const
  {
    return glm::perspective(glm::radians(Zoom), aspectRatio, 0.01f, 5000.0f);
  }

  void processKeyboard(CameraMovement direction, float deltaTime)
  {

    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
    {
      Position += Front * velocity;
    }
    if (direction == BACKWARD)
    {
      Position -= Front * velocity;
    }
    if (direction == LEFT)
    {
      Position -= Right * velocity;
    }
    if (direction == RIGHT)
    {
      Position += Right * velocity;
    }
    if (direction == UP)
    {
      Position += WorldUp * velocity;
    }
    if (direction == DOWN)
    {
      Position -= WorldUp * velocity;
    }
  }

  void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
  {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
      if (Pitch > 89.0f)
        Pitch = 89.0f;
      if (Pitch < -89.0f)
        Pitch = -89.0f;
    }

    updateCameraVectors();
  }

  void invertPitch()
  {
    Pitch = -Pitch;
    updateCameraVectors();
  }

  void processMouseScroll(float yoffset)
  {
    Zoom -= yoffset;
    if (Zoom < 1.0f)
      Zoom = 1.0f;
    if (Zoom > 90.0f)
      Zoom = 90.0f;
  }

  Frustum extractFrustumPlanes(const glm::mat4 &viewProj) const
  {
    Frustum frustum;
    glm::mat4 m = viewProj;

    frustum.planes[0] = glm::vec4(
        m[0][3] + m[0][0],
        m[1][3] + m[1][0],
        m[2][3] + m[2][0],
        m[3][3] + m[3][0]);

    frustum.planes[1] = glm::vec4(
        m[0][3] - m[0][0],
        m[1][3] - m[1][0],
        m[2][3] - m[2][0],
        m[3][3] - m[3][0]);

    frustum.planes[2] = glm::vec4(
        m[0][3] + m[0][1],
        m[1][3] + m[1][1],
        m[2][3] + m[2][1],
        m[3][3] + m[3][1]);

    frustum.planes[3] = glm::vec4(
        m[0][3] - m[0][1],
        m[1][3] - m[1][1],
        m[2][3] - m[2][1],
        m[3][3] - m[3][1]);

    frustum.planes[4] = glm::vec4(
        m[0][3] + m[0][2],
        m[1][3] + m[1][2],
        m[2][3] + m[2][2],
        m[3][3] + m[3][2]);

    frustum.planes[5] = glm::vec4(
        m[0][3] - m[0][2],
        m[1][3] - m[1][2],
        m[2][3] - m[2][2],
        m[3][3] - m[3][2]);

    for (int i = 0; i < 6; i++)
    {
      float length = glm::length(glm::vec3(frustum.planes[i]));
      frustum.planes[i] /= length;
    }

    return frustum;
  }

private:
  void updateCameraVectors()
  {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif
