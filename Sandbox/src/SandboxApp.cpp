// CLIENT

#include <GameEngine.h>

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
		PushOverlay(new ge::ImGuiLayer());
	}

	~Sandbox() {

	}
};

ge::Application* ge::CreateApplication() {

	return new Sandbox();
}