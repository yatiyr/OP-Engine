#pragma once

#define GLM_FORCE_AVX
#define GLM_FORCE_SSE2

#include <stdio.h>

// Applications using Opium Engine library use this header
#include <Opium/Application.h>
#include <Opium/Logging.h>
#include <Opium/Layer.h>

#include <Opium/Timestep.h>

#include <Input/Input.h>
#include <Input/KeyCodes.h>
#include <Input/MouseButtonCodes.h>

#include <Gui/ImGuiLayer.h>

#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Components.h>

#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/RenderCommand.h>

#include <Renderer/Buffer.h>
#include <Renderer/Shader.h>
#include <Renderer/Framebuffer.h>
#include <Renderer/Texture.h>
#include <Renderer/SubTexture2D.h>
#include <Renderer/VertexArray.h>

#include <Renderer/OrthographicCamera.h>
#include <Renderer/OrthographicCameraController.h>