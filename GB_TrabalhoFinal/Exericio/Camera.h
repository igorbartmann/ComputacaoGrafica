#pragma once

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Camera
{
public:
	Camera()
	{
		yaw = -90.0;;
		pitch = 0.0f;
		cameraStartPosition = true;
		cameraSpeed = 0.25f;
		cameraSensitivity = 0.05f;
		mousePositionLastX = 0.0f;
		mousePositionLastY = 0.0f;
	}

	void initialize(float window_width, float window_height)
	{
		yaw = -90.0;;
		pitch = 0.0f;
		cameraStartPosition = true;
		cameraSpeed = 0.25f;
		cameraSensitivity = 0.05f;
		mousePositionLastX = 0.0f;
		mousePositionLastY = 0.0f;

		cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		projection = glm::perspective(glm::radians(45.0f), window_width / window_height, 0.1f, 100.0f);
	}

	glm::mat4 getCameraView()
	{
		return view;
	}
	void recalculateCameraView()
	{
		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
	}

	glm::mat4 getCameraProjection()
	{
		return projection;
	}

	void setCameraProjection(glm::vec3 x, glm::vec3 y, glm::vec3 z)
	{
		view = glm::lookAt(x, y, z);
	}

	glm::vec3 getCameraPosition()
	{
		return cameraPosition;
	}

	void setCameraPosition(glm::vec3 new_cameraPosition)
	{
		cameraPosition = new_cameraPosition;
	}

	void moveFront()
	{
		cameraPosition += cameraFront * cameraSpeed;
	}
	void moveBack()
	{
		cameraPosition -= cameraFront * cameraSpeed;
	}
	void moveRight()
	{
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	void moveLeft()
	{
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	void updateMatrixByMousePosition(double xpos, double ypos)
	{
		if (cameraStartPosition)
		{
			mousePositionLastX = xpos;
			mousePositionLastY = ypos;
			cameraStartPosition = false;
		}

		float offsetx = xpos - mousePositionLastX;
		float offsety = mousePositionLastY - ypos;

		mousePositionLastX = xpos;
		mousePositionLastY = ypos;

		offsetx *= cameraSensitivity;
		offsety *= cameraSensitivity;

		pitch += offsety;
		yaw += offsetx;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		else if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);

		glm::vec3 right = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		cameraUp = glm::normalize(glm::cross(right, cameraFront));
	}

protected:
	float yaw;
	float pitch;
	bool cameraStartPosition;
	float cameraSpeed;
	float cameraSensitivity;
	float mousePositionLastX;
	float mousePositionLastY;

	glm::vec3 cameraPosition;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	glm::mat4 view;
	glm::mat4 projection;
};