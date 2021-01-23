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


		/*auto mainCamera = std::make_shared<Core::CameraPerspective>("mainCamera");
		mainCamera->SetPosition(0.0f, 0.0f, -10.0f); //3Cylinder_Big
		mainCamera->LookAt(mainCamera->GetPosition(), float3(0.0f, 0.0f, 0.0f), float3(0.0, 1.0, 0.0));
		mainCamera->SetLens(DirectX::XM_PIDIV4, static_cast<float>(m_ClientWidth) / m_ClientHeight, 10.0f, 1000.0f);
		mainCamera->SetSpeed(0.1f);
		Core::CameraManager::GetInstance()->AddCamera(mainCamera);*/

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
		auto dirLight0 = lightManager->AddLightDirectional("dlight_0", float4{ 0.8f, 0.8f, 0.8f, 0.8f }, float4{ 0.3f, 0.3f, 0.3f, 0.3f }, float4{ 0.0f, 0.0f, 0.0f, 0.0f }, float3{.38f, -.60f, .71f}, float3{ -42.0f, 66.0f, -80.0f }, 0.05f, true, 200.0f);
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
	}


	void ApplicationEditor::OnUpdate()
	{

		TE_INSTANCE_PHYSICSENGINE->Simulate(m_Timer.DeltaTime());


		Core::InputManager::ProcessInput();


		m_RendererLayer->BeginRendering();


		for (auto layer : m_LayerStack)
		{
			layer->OnUpdate(m_Timer.DeltaTime());
		}


		if (m_RendererLayer->BeginImGuiLayer())
		{
			OnImGuiRender();
			for (auto layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_RendererLayer->EndImGuiLayer();
		}


		m_RendererLayer->EndRendering();
	}


	void ApplicationEditor::OnProcess()
	{
	}


	void ApplicationEditor::OnDestroy()
	{
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
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			auto dockSpaceID = ImGui::GetID("MainDockSpace");

			ImGui::DockSpace(dockSpaceID);

			//Render Scene Viewport
			{
				ImGui::Begin("SceneViewport", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

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

						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "CPU: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%s", sysInfo.mCPUModel.c_str());
						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "Logical Processors: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%i", sysInfo.mThreadNum);
						ImGui::TextColored(ImVec4{ 0.972, 0.925, 0.407, 1.0f }, "Installed RAM: "); ImGui::SameLine(110.0); ImGui::TextColored(ImVec4{ 1, 0.980, 0.760, 1.0f }, "%i GB", sysInfo.mInstalledRAM);

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

						ImGui::TextColored(ImVec4{ 0.949, 0.129, 0.329, 1.0f }, "GPU: "); ImGui::SameLine(90.0); ImGui::TextColored(ImVec4{ 1, 0.658, 0.745,1.0f }, "%s", sysInfo.mGPUDesc.c_str());
						ImGui::TextColored(ImVec4{ 0.949, 0.129, 0.329, 1.0f }, "GPU Memory: "); ImGui::SameLine(90.0); ImGui::TextColored(ImVec4{ 1, 0.658, 0.745,1.0f }, "%i GB", sysInfo.mGPUMemory);
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


}

TruthEngine::Core::Application* TruthEngine::Core::CreateApplication() {
	return new TruthEngine::ApplicationEditor(1366, 1000, 2);
}