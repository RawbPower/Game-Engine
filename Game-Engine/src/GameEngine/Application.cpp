#include "Application.h"

#include "GameEngine/Events/ApplicationEvent.h"
#include "GameEngine/Log.h"

namespace ge {

	Application::Application()
	{
	}


	Application::~Application()
	{
	}

	void Application::Run() {

		WindowResizeEvent e(1280, 700);
		if (e.IsInCategory(EventCategoryApplication)) {
			GE_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput)) {
			GE_TRACE(e);
		}

		while (true);
	}
}
