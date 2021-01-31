#include "pch.h"
#include "EditorApp.h"

#include "Core/Entity/Light/LightManager.h"
#include "Core/Entity/Light/ILight.h"
#include "Core/Entity/Light/LightDirectional.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Input/InputManager.h"
#include "Core/Entity/Camera/CameraManager.h"
#include "Core/Renderer/GraphicDevice.h"
#include "Core/Entity/Components.h"
#include "Core/Helper/HardwareInfo.h"
#include "Core/Renderer/TextureMaterialManager.h"
#include "Core/Renderer/TextureMaterial.h"
#include "Core/Renderer/RendererLayer.h"
#include "Core/ImGui/ImGuiLayer.h"
#include "Core/Event/EventKey.h"
#include "Core/Event/Event.h"
#include "Core/Entity/PickingEntity.h"

std::future<void> gFeaturePickEntity;

namespace TruthEngine
{

	using namespace Core;

	ApplicationEditor::ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum)
		: Core::Application("TruthEngine.Editor", clientWidth, clientHeight, framesInFlightNum)
	{
	}


	void ApplicationEditor::OnInit()
	{

		Core::InputManager::RegisterKey('A');
		Core::InputManager::RegisterKey('W');
		Core::InputManager::RegisterKey('S');
		Core::InputManager::RegisterKey('D');
		Core::InputManager::RegisterKey(Key::Space);


		auto mainCamera = Core::CameraManager::GetInstance()->CreatePerspectiveFOV("mainCamera"
			, float3{ -12.0f, 17.0f, -30.0f }
			, float3{ 0.48f, -.5f, 0.72f }
			, float3{ 0.0f, 1.0f, 0.0f }
			, DirectX::XM_PIDIV4
			, static_cast<float>(m_ClientWidth) / m_ClientHeight
			, 1.0f
			, 1000.0f
		);


		Core::CameraManager::GetInstance()->SetActiveCamera(mainCamera);


		auto entity = m_ActiveScene.AddEntity("MainCamera");
		entity.AddComponent<Core::CameraComponent>(mainCamera, Core::CameraManager::GetInstance()->GetCameraController());


		auto lightManager = Core::LightManager::GetInstace();
		auto dirLight0 = lightManager->AddLightDirectional("dlight_0", float4{ 0.8f, 0.8f, 0.8f, 0.8f }, float4{ 0.3f, 0.3f, 0.3f, 0.3f }, float4{ 0.0f, 0.0f, 0.0f, 0.0f }, float3{ .38f, -.60f, .71f }, float3{ -42.0f, 66.0f, -80.0f }, 0.05f, true, 200.0f);
		lightManager->AddLightCamera(dirLight0, Core::TE_CAMERA_TYPE::Perspective);

		auto entityLight = m_ActiveScene.AddEntity("Directional Light 0");
		entityLight.AddComponent<LightComponent>(dirLight0);


		m_LayerStack.PushLayer(m_RendererLayer.get());


		//must put ModelManager initiation after RendererLayer attachment so that the bufferManager has been initiated 
		auto modelManager = Core::ModelManager::GetInstance().get();
		modelManager->Init(TE_INSTANCE_BUFFERMANAGER.get());
		//modelManager->AddSampleModel();


		m_SceneHierarchyPanel.SetContext(&m_ActiveScene);


		TE_INSTANCE_PHYSICSENGINE->Init();

		RegisterOnEvents();
	}


	void ApplicationEditor::OnUpdate()
	{


		TE_INSTANCE_PHYSICSENGINE->Simulate(m_Timer.DeltaTime());

		m_TimerAvg_ImGuiPass.Start();

		Core::InputManager::ProcessInput();


		m_RendererLayer->BeginRendering();


		m_TimerAvg_UpdateRenderPasses.Start();

		for (auto layer : m_LayerStack)
		{
			layer->OnUpdate(m_Timer.DeltaTime());
		}

		m_TimerAvg_UpdateRenderPasses.End();



		if (m_RendererLayer->BeginImGuiLayer())
		{
			OnImGuiRender();
			for (auto layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_RendererLayer->EndImGuiLayer();
		}

		m_TimerAvg_ImGuiPass.End();


		m_TimerAvg_Update.Start();

		m_RendererLayer->EndRendering();

		m_TimerAvg_Update.End();
	}


	void ApplicationEditor::OnProcess()
	{
	}


	void ApplicationEditor::OnDestroy()
	{
	}

	void ApplicationEditor::OnEventKeyReleased(EventKeyReleased& event)
	{
		if (event.GetKeyCode() == Key::Space)
		{
			m_Window->ToggleFullScreenMode();
		}
	}

	void ApplicationEditor::OnImGuiRender()
	{
		static auto imguiLayer = m_RendererLayer->GetImGuiLayer();
		static ImGui::FileBrowser fileBrowserImportModel;
		static ImGui::FileBrowser fileBrowserImportTexture;
		static bool importDiffuseTexture = false;
		static bool importNormalTexture = false;

		auto mainViewPort = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(mainViewPort->ID);
		ImGui::SetNextWindowPos(mainViewPort->Pos);
		ImGui::SetNextWindowSize(mainViewPort->Size);

		static ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("TruthEngine", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); //Main Engine Window
		ImGui::PopStyleVar(3);
		{

			// Menu Bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open"))
					{
					}
					if (ImGui::MenuItem("Reset"))
					{
					}
					if (ImGui::BeginMenu("Import"))
					{
						if (ImGui::MenuItem("Model3D"))
						{
							static const std::vector<const char*> fileExtensions = { ".obj", ".fbx" };
							imguiLayer->OpenFileDialog(&fileBrowserImportModel, "Open Model", fileExtensions);
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Test"))
				{
					if (ImGui::MenuItem("Show ImGui Demo Window"))
					{
						static bool s_showDemoWindow = false;
						s_showDemoWindow = !s_showDemoWindow;
						imguiLayer->ShowDemoWindow(s_showDemoWindow);
					}
					if (ImGui::MenuItem("Show MaterialTextures Window"))
					{
						auto dummyFunc = [](uint32_t i) {};
						ImGuiLayer::ShowWindowMaterialTexture(dummyFunc, true);
					}
					if (ImGui::MenuItem("Generate Plane"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::PLANE);
					}
					if (ImGui::MenuItem("Generate Box"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::BOX);
					}
					if (ImGui::MenuItem("Generate RoundedBox"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::ROUNDEDBOX);
					}
					if (ImGui::MenuItem("Generate Sphere"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::SPHERE);
					}
					if (ImGui::MenuItem("Generate Cylinder"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::CYLINDER);
					}
					if (ImGui::MenuItem("Generate Capped Cylinder"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::CAPPEDCYLINDER);
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Physics"))
				{
					if (ImGui::MenuItem("Run Physics"))
					{
						TE_INSTANCE_PHYSICSENGINE->Play();
					}
					if (ImGui::MenuItem("Stop Physics"))
					{
						TE_INSTANCE_PHYSICSENGINE->Stop();
					}
					if (ImGui::MenuItem("Reset Physics"))
					{
						TE_INSTANCE_PHYSICSENGINE->Reset();
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			auto dockSpaceID = ImGui::GetID("MainDockSpace");

			ImGui::DockSpace(dockSpaceID);

			//Render Scene Viewport
			{
				ImGui::Begin("SceneViewport", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
				m_IsHoveredSceneViewport = ImGui::IsWindowHovered();
				auto _viewportSize = ImGui::GetContentRegionAvail();


				if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_IsHoveredSceneViewport)
				{
					auto dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
					if (dragDelta.x == 0 && dragDelta.y == 0)
					{
						m_ActiveScene.ClearSelectedEntity();
						m_SceneViewPortPos = ImGui::GetWindowPos();
						auto _mousePos = ImGui::GetMousePos();
						m_SceneViewPortAreaMin = ImGui::GetWindowContentRegionMin();
						m_SceneViewPortAreaMax = ImGui::GetWindowContentRegionMax();
						

						auto _windowMousePos = float2{ _mousePos.x - m_SceneViewPortPos.x - m_SceneViewPortAreaMin.x, _mousePos.y - m_SceneViewPortPos.y - m_SceneViewPortAreaMin.y };

						if ((_windowMousePos.x > 0 && _windowMousePos.y > 0) && (_windowMousePos.x < m_SceneViewPortAreaMax.x && _windowMousePos.y < m_SceneViewPortAreaMax.y))
						{

							/*std::function<void()> lambda = [this, _windowMousePos]() { PickingEntity::PickEntity(_windowMousePos, &m_ActiveScene, CameraManager::GetInstance()->GetActiveCamera()); };

							gFeaturePickEntity = TE_INSTANCE_THREADPOOL.Queue(lambda);*/

							PickingEntity::PickEntity(_windowMousePos, &m_ActiveScene, CameraManager::GetInstance()->GetActiveCamera());
						}
					}
				}

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				auto windowPos = ImGui::GetWindowPos();
				ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

				auto viewportSize = ImGui::GetContentRegionAvail();
				if (viewportSize.x != GetSceneViewportWidth() || viewportSize.y != GetSceneViewportHeight())
				{
					if (viewportSize.x > 1 && viewportSize.y > 1)
					{
						ResizeSceneViewport(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
					}
				}

				imguiLayer->RenderSceneViewport(viewportSize);



				ImGui::End();
			}//End of Render Scene Viewport


			//Render scene Hierarchy Panel
			{
				ImGui::Begin("Scene Panel");

				m_SceneHierarchyPanel.OnImGuiRender();

				ImGui::End();
			}//End of Render scene Hierarchy Panel


			//Render Property Panel
			{
				ImGui::Begin("Properties");

				m_EntityPropertyPanel.SetContext(m_ActiveScene.GetSelectedEntity());
				m_EntityPropertyPanel.OnImGuiRender();

				ImGui::End();
			}//End of Render Property Panel


			//Render Setting Window
			{
				ImGui::Begin("Configuration");

				if (ImGui::BeginTable("##renderersettingsysinfotable", 2, ImGuiTableFlags_ColumnsWidthFixed))
				{
					ImGui::TableSetupColumn("System Informations");
					ImGui::TableSetupColumn("Profiler", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					auto sysInfo = HardwareInfo::GetSystemInfo();
					if (ImGui::BeginTable("##renderersettingcpuinfotable", 2, ImGuiTableFlags_ColumnsWidthFixed))
					{

						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						static std::string _CpuModel = sysInfo.mCPUModel;
						static auto _CpuThreadNum = sysInfo.mThreadNum;
						static auto _InstalledRAM = sysInfo.mInstalledRAM;

						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "CPU: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%s", _CpuModel.c_str());
						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "Logical Processors: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%i", _CpuThreadNum);
						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "Installed RAM: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%i GB", _InstalledRAM);

						ImGui::TableNextColumn();

						std::string api = "";
						switch (Settings::RendererAPI)
						{
						case TE_RENDERER_API::DirectX12:
							api = "DirectX 12";
							break;
						case TE_RENDERER_API::DirectX11:
							api = "DirectX";
						default:
							break;
						}

						static std::string _GPUDesc = sysInfo.mGPUDesc;
						static auto _GPUMemory = sysInfo.mGPUMemory;

						ImGui::TextColored(ImVec4{ 0.949, 0.129, 0.329, 1.0f }, "GPU: "); ImGui::SameLine(90.0); ImGui::TextColored(ImVec4{ 1, 0.658, 0.745,1.0f }, "%s", _GPUDesc.c_str());
						ImGui::TextColored(ImVec4{ 0.949, 0.129, 0.329, 1.0f }, "GPU Memory: "); ImGui::SameLine(90.0); ImGui::TextColored(ImVec4{ 1, 0.658, 0.745,1.0f }, "%i GB", _GPUMemory);
						ImGui::TextColored(ImVec4{ 0.949, 0.129, 0.329, 1.0f }, "Graphic API: "); ImGui::SameLine(90.0); ImGui::TextColored(ImVec4{ 1, 0.658, 0.745,1.0f }, "%s", api.c_str());

						ImGui::EndTable();
					}

					ImGui::TableNextColumn();

					if (ImGui::BeginTable("##renderersettingprofilertable", 2))
					{
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						ImGui::TextColored(ImVec4{ 0.529, 0.952, 0.486 ,1.0f }, "Frame Per Second:");
						ImGui::TableNextColumn();
						ImGui::TextColored(ImVec4{ 0.529, 0.952, 0.486 ,1.0f }, "%u", m_Timer.GetFPS());

						ImGui::TableNextRow();
						ImGui::TableNextColumn();

						ImGui::TextColored(ImVec4{ 0.529, 0.952, 0.486 ,1.0f }, "CPU Frame Time: ");
						ImGui::TableNextColumn();
						ImGui::TextColored(ImVec4{ 0.529, 0.952, 0.486 ,1.0f }, "%.3f", m_Timer.GetAverageCpuTime());
						ImGui::Text("RenderPass Time: %0.3f", m_TimerAvg_UpdateRenderPasses.GetAverageTime());
						ImGui::Text("ImGUiPass Time: %0.3f", m_TimerAvg_ImGuiPass.GetAverageTime());
						ImGui::Text("AppUpdate Time: %0.3f", m_TimerAvg_Update.GetAverageTime());

						ImGui::EndTable();
					}

					ImGui::EndTable();

				}

				/*if (ImGui::BeginTable("##renderersettingtable", 3, ImGuiTableFlags_ColumnsWidthFixed))
				{
					auto sysInfo = HardwareInfo::GetSystemInfo();

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::Text("CPU: %s", sysInfo.mCPUModel.c_str());
					ImGui::Text("Logical Processors: %i", sysInfo.mThreadNum);
					ImGui::Text("Installed RAM: %i GB", sysInfo.mInstalledRAM);

					ImGui::TableNextColumn();

					std::string api = "";
					switch (Settings::RendererAPI)
					{
					case TE_RENDERER_API::DirectX12:
						api = "DirectX 12";
						break;
					case TE_RENDERER_API::DirectX11 :
						api = "DirectX";
					default:
						break;
					}

					ImGui::Text("GPU: %s", sysInfo.mGPUDesc.c_str());
					ImGui::Text("GPU Memory: %i GB", sysInfo.mGPUMemory);
					ImGui::Text("Graphic API: %s", api.c_str());


					ImGui::TableNextColumn();

					ImGui::Text("Frame Per Second: %u", m_Timer.GetFPS());
					ImGui::Text("CPU Frame Time: %.3f", m_Timer.GetAverageCpuTime());

					ImGui::EndTable();
				}*/



				ImGui::End();
			}//End of Render Property Panel


			/*if (ImGui::Begin("RenderingTextures", NULL, ImGuiWindowFlags_MenuBar))
			{
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Import"))
					{
						if (ImGui::MenuItem("Diffuse"))
						{
							static const std::vector<const char*> fileExtensions = { ".jpg", ".jpeg", ".png" };
							imguiLayer->OpenFileDialog(&fileBrowserImportTexture, "Open Texture", fileExtensions);
							importDiffuseTexture = true;
							importNormalTexture = false;
						}
						if (ImGui::MenuItem("Normal"))
						{
							static const std::vector<const char*> fileExtensions = { ".jpg", ".jpeg", ".png" };
							imguiLayer->OpenFileDialog(&fileBrowserImportTexture, "Open Texture", fileExtensions);
							importNormalTexture = true;
							importDiffuseTexture = false;
						}
						if (ImGui::MenuItem("Displacement"))
						{
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				auto textures = TextureMaterialManager::GetInstance()->GetAllTextures();

				for (auto& t : textures)
				{
					ImGui::SameLine();
					TEImGuiRenderImage_MaterialTexture(t->GetViewIndex(), float2{ 150, 150 });
				}

				ImGui::End();
			}*/

			ImGui::End();
		}//End of main window: Begin("TruthEngine")

		//
		//Ckeck on selected file for importing 3d model
		//
		static std::string importFilePath;
		if (imguiLayer->CheckFileDialog(&fileBrowserImportModel, importFilePath))
		{
			Core::GraphicDevice::GetPrimaryDevice()->WaitForGPU();
			Core::ModelManager::GetInstance()->ImportModel(&m_ActiveScene, importFilePath.c_str());
		}
		if (imguiLayer->CheckFileDialog(&fileBrowserImportTexture, importFilePath))
		{
			auto tex = TextureMaterialManager::GetInstance()->CreateTexture(importFilePath.c_str(), "");
		}

	}


	void ApplicationEditor::RegisterOnEvents()
	{
		auto onKeyReleased = [this](Event& event)
		{
			OnEventKeyReleased(static_cast<EventKeyReleased&>(event));
		};

		RegisterEventListener(EventType::KeyReleased, onKeyReleased);

		
	}

}

TruthEngine::Core::Application* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::ApplicationEditor(1366, 1000, 3);
}