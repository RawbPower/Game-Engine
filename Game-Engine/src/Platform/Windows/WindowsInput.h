/*
	Window Input

	Class used to inherit from Input for inputs used in the application windows
*/

#pragma once

#include "GameEngine/Core/Input.h"

namespace ge {

	class WindowsInput : public Input 
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}