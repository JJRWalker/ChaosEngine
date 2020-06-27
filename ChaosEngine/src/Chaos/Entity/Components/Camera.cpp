#include "chaospch.h"

#include "Camera.h"
#include "Chaos/Core/Application.h"

#include <GLM/glm/gtc/matrix_transform.hpp>

namespace Chaos
{
	void Camera::Recalculate()
	{
		mView = glm::lookAt(glm::vec3(mPosition.X, mPosition.Y, mPosition.Z + 1), glm::vec3(mPosition.X, mPosition.Y, mPosition.Z), glm::vec3(0.0f, 1.0f, 0.0f));


		mProjection = glm::ortho(mBounds.X * (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(),
			mBounds.Y * (float)Application::Get().GetWindow().GetWidth() / (float)Application::Get().GetWindow().GetHeight(),
			mBounds.Z, mBounds.W, -5.0f, 5.0f);
		mProjection[1][1] *= -1;
	}
}