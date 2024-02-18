#pragma once
// Header file only intended to be included by client applications

//#include <stdio.h>
#include "GameEngine/Core/Application.h"
#include "GameEngine/Core/Layer.h"
#include "GameEngine/Core/Log.h"

#include "GameEngine/Core/DeltaTime.h"

#include "GameEngine/Core/Input.h"
#include "GameEngine/Core/MouseButtonCodes.h"
#include "GameEngine/Core/KeyCodes.h"
#include "GameEngine/Renderer/OrthographicCameraController.h"
#include "GameEngine/Renderer/PerspectiveCameraController.h"
#include "GameEngine/Renderer/Model.h"
#include "GameEngine/Renderer/Framebuffer.h"
#include "GameEngine/Core/Scene.h"

#include "GameEngine/Math/Quat.h"
#include "GameEngine/Math/Matrix.h"
#include "GameEngine/Math/Vector.h"

#include "GameEngine/Physics/Body.h"

#include "GameEngine/ImGui/ImGuiLayer.h"

// ---Renderer--------------------
#include "GameEngine/Renderer/Renderer.h"
#include "GameEngine/Renderer/RenderCommand.h"

#include "GameEngine/Renderer/Buffer.h"
#include "GameEngine/Renderer/Shader.h"
#include "GameEngine/Renderer/Texture.h"
#include "GameEngine/Renderer/VertexArray.h"

#include "GameEngine/Renderer/OrthographicCamera.h"
#include "GameEngine/Renderer/PerspectiveCamera.h"

#include "GameEngine/Renderer/Lighting.h"
//--------------------------------

// ---Entry Point-----------------
#include "GameEngine/Core/EntryPoint.h"
//--------------------------------