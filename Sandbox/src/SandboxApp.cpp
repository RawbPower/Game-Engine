// CLIENT

#include <GameEngine.h>

class Sandbox : public ge::Application
{
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

ge::Application* ge::CreateApplication() {

	return new Sandbox();
}