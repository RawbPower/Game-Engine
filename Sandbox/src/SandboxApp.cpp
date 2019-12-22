// CLIENT

#include <GameEngine.h>

#include "imgui/imgui.h"

class ExampleLayer : public ge::Layer {
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//GE_INFO("ExampleLayer::Update");

		if (ge::Input::IsKeyPressed(GE_KEY_TAB))
			GE_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(ge::Event& event) override
	{
		if (event.GetEventType() == ge::EventType::KeyPressed)
		{
			ge::KeyPressedEvent& e = (ge::KeyPressedEvent&)event;
			if (e.GetKeyCode() == GE_KEY_TAB)
				GE_TRACE("Tab key is pressed (event)!");
			GE_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};


class Sandbox : public ge::Application
{
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

ge::Application* ge::CreateApplication() {

	return new Sandbox();
}