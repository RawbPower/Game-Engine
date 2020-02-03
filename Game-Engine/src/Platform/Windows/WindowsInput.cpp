/*
	Window Input

	Class used to inherit from Input for inputs used in the application windows.
	Uses glfw to recognise inputs
*/

#include "gepch.h"
#include "WindowsInput.h"

// It's fine to this as we a inside a platform specific file
#include "GameEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace ge 
{

	Input* Input::s_Instance = new WindowsInput();

	// Check if certain key is pressed
	bool  WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
						
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	// Check if mouse button is pressed
	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	// Get the position of the cursor in the window
	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	// Get the X position of the cursor in the window
	float WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePositionImpl();

		return x;
	}

	// Get the Y position of the cursor in the window
	float WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePositionImpl();

		return y;
	}

}