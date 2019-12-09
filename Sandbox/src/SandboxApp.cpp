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
		GE_INFO("ExampleLayer::Update");
	}

	void OnEvent(ge::Event& event) override
	{
		GE_TRACE("{0}", event);
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