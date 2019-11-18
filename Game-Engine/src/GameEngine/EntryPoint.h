#pragma once

#ifdef GE_PLATFORM_WINDOWS

extern ge::Application* ge::CreateApplication();
	
int main(int argc, char** argv) {
	//printf("Game Engine\n");
	auto app = ge::CreateApplication();
	app->Run();
	delete app;
}

#endif