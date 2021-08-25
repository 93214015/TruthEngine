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
#include "Core/Event/EventEntity.h"
#include "Core/AnimationEngine/AnimationManager.h"
#include "Core/Entity/Scene.h"

std::future<void> gFeaturePickEntity;


TruthEngine::Application* TruthEngine::CreateApplication() {
	return new TruthEngine::ApplicationEditor(1920, 1000, 3);
}

namespace TruthEngine
{

	ApplicationEditor::ApplicationEditor(uint16_t clientWidth, uint16_t clientHeight, uint8_t framesInFlightNum)
		: Application("TruthEngine.Editor", clientWidth, clientHeight, framesInFlightNum)
	{
	}


	void ApplicationEditor::OnInit()
	{

		InputManager::RegisterKey('A');
		InputManager::RegisterKey('W');
		InputManager::RegisterKey('S');
		InputManager::RegisterKey('D');
		InputManager::RegisterKey(Key::Space);


		auto mainCamera = TE_INSTANCE_CAMERAMANAGER->CreatePerspectiveFOV("mainCamera"
			, float3{ -12.0f, 17.0f, -30.0f }
			, float3{ 0.48f, -.5f, 0.72f }
			, float3{ 0.0f, 1.0f, 0.0f }
			, DirectX::XM_PIDIV4
			, static_cast<float>(m_ClientWidth) / static_cast<float>(m_ClientHeight)
			, 1.0f
			, 1000.0f
			, false
		);

		TE_INSTANCE_CAMERAMANAGER->SetActiveCamera(mainCamera);

		GetActiveScene()->Init();

		//
		//Add Main Camera
		//
		auto entity = GetActiveScene()->AddEntity("MainCamera");
		entity.AddComponent<CameraComponent>(mainCamera, TE_INSTANCE_CAMERAMANAGER->GetCameraController());

		m_LayerStack.PushLayer(m_RendererLayer.get());

		//
		//Adding Lights
		//
		/*const float4 _cascadeCoveringPercentage = float4{ 10.0f, 50.0f, 100.f, 200.0f };
		GetActiveScene()->AddLightEntity_Directional("SunLight", float3{ 0.0f, 0.0f, 0.0f }, float3{ .38f, -.60f, .71f }, float3{ -42.0f, 66.0f, -80.0f }, 0.05f, true, _cascadeCoveringPercentage);*/
		GetActiveScene()->AddLightEntity_Spot("SpotLight0", float3{ .8f, .8f, .8f }, float3{ .01f, -.71f, .7f }, float3{ -0.3f, 28.0f, -42.0f }, 0.05f, true, 90.0f, 200.0f, 20.0f, 60.0f);

		//auto lightManager = LightManager::GetInstace();
		//auto dirLight0 = lightManager->AddLightDirectional("dlight_0", float3{ 0.8f, 0.8f, 0.8f }, float3{ .38f, -.60f, .71f }, float3{ -42.0f, 66.0f, -80.0f }, 0.05f, true, _cascadeCoveringPercentage);
		////lightManager->AddLightCamera(dirLight0, TE_CAMERA_TYPE::Perspective);
		//auto entityLight = GetActiveScene()->AddEntity("Directional Light 0");
		//entityLight.AddComponent<LightComponent>(dirLight0);

		//must put ModelManager initiation after RendererLayer attachment so that the bufferManager has been initiated 
		auto modelManager = ModelManager::GetInstance();
		modelManager->Init(TE_INSTANCE_BUFFERMANAGER);
		//modelManager->AddSampleModel();

		m_SceneHierarchyPanel.SetContext(GetActiveScene());

		TE_INSTANCE_PHYSICSENGINE->Init();

		GetActiveScene()->AddEnvironmentEntity();

		RegisterOnEvents();
	}


	void ApplicationEditor::OnUpdate()
	{

		TE_INSTANCE_PHYSICSENGINE->Simulate(m_Timer.DeltaTime());

		TE_INSTANCE_ANIMATIONMANAGER->Update(m_Timer.DeltaTime());

		GetActiveScene()->OnUpdate(m_Timer.DeltaTime());

		InputManager::ProcessInput();

		m_RendererLayer->BeginRendering();

		m_TimerAvg_UpdateRenderPasses.Start();

		for (auto layer : m_LayerStack)
		{
			layer->OnUpdate(m_Timer.DeltaTime());
		}

		m_TimerAvg_UpdateRenderPasses.End();

		m_TimerAvg_ImGuiPass.Start();

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
							static const std::vector<const char*> fileExtensions = { ".obj", ".fbx", ".3ds", ".dae", ".blend", ".gltf" };
							imguiLayer->OpenFileDialog(&fileBrowserImportModel, "Open Model", fileExtensions);
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Test"))
					{
						static bool _Checked = false;
						ImGui::Checkbox("Rotation", &_Checked);

						if (_Checked)
						{
							Entity _SelectedEntity = GetActiveScene()->GetSelectedEntity();
							if (_SelectedEntity)
							{
								float angle = DirectX::XMConvertToRadians(0.25f * static_cast<float>(InputManager::GetDX()));
								angle /= 10.0f;

								float4x4A& _Transform = _SelectedEntity.GetComponent<TransformComponent>().GetTransform();

								//float3 _RotationOrigin = float3{ _Transform._41, _Transform._42, _Transform._43 };
								float3 _RotationOrigin = float3{ 0, 0, 0 };

								float4x4A _RotationTransform = Math::TransformMatrixRotation(angle, float3{ .0f, 1.0f, .0f }, _RotationOrigin);

								_Transform = _Transform * _RotationTransform;
							}
						}

						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Exit"))
					{
						exit(0);
					}
					ImGui::EndMenu();
				}

				/*if (ImGui::BeginMenu("Test"))
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
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::PLANE, "DefaultModel");
					}
					if (ImGui::MenuItem("Generate Box"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::BOX, "DefaultModel");
					}
					if (ImGui::MenuItem("Generate RoundedBox"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::ROUNDEDBOX, "DefaultModel");
					}
					if (ImGui::MenuItem("Generate Sphere"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::SPHERE, "DefaultModel");
					}
					if (ImGui::MenuItem("Generate Cylinder"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::CYLINDER, "DefaultModel");
					}
					if (ImGui::MenuItem("Generate Capped Cylinder"))
					{
						ModelManager::GetInstance()->GeneratePrimitiveMesh(TE_PRIMITIVE_TYPE::CAPPEDCYLINDER, "DefaultModel");
					}
					ImGui::EndMenu();
				}*/

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
						GetActiveScene()->ClearSelectedEntity();
						m_SceneViewPortPos = ImGui::GetWindowPos();
						auto _mousePos = ImGui::GetMousePos();
						m_SceneViewPortAreaMin = ImGui::GetWindowContentRegionMin();
						m_SceneViewPortAreaMax = ImGui::GetWindowContentRegionMax();
						

						auto _windowMousePos = float2{ _mousePos.x - m_SceneViewPortPos.x - m_SceneViewPortAreaMin.x, _mousePos.y - m_SceneViewPortPos.y - m_SceneViewPortAreaMin.y };

						if ((_windowMousePos.x > 0 && _windowMousePos.y > 0) && (_windowMousePos.x < m_SceneViewPortAreaMax.x && _windowMousePos.y < m_SceneViewPortAreaMax.y))
						{

							/*std::function<void()> lambda = [this, _windowMousePos]() { PickingEntity::PickEntity(_windowMousePos, &GetActiveScene(), CameraManager::GetInstance()->GetActiveCamera()); };

							gFeaturePickEntity = TE_INSTANCE_THREADPOOL.Queue(lambda);*/

							PickingEntity::PickEntity(_windowMousePos, GetActiveScene(), TE_INSTANCE_CAMERAMANAGER->GetActiveCamera());
						}
					}
				}

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				auto windowPos = ImGui::GetWindowPos();
				ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

				auto _ContentRegionAvailable = ImGui::GetContentRegionAvail();
				uint2 viewportSize{ static_cast<uint32_t>(_ContentRegionAvailable.x), static_cast<uint32_t>(_ContentRegionAvailable.y) };
				if (viewportSize.x != GetSceneViewportWidth() || viewportSize.y != GetSceneViewportHeight())
				{
					if (viewportSize.x > 1 && viewportSize.y > 1)
					{
						ResizeSceneViewport(viewportSize.x, viewportSize.y);
					}
				}

				imguiLayer->RenderSceneViewport(_ContentRegionAvailable);


				ImGui::End();
			}//End of Render Scene Viewport


			//Render Scene Property Panel
			{
				ImGui::Begin("Scene Properties");




				static bool _IsEnvironmentMapEnabled = m_RendererLayer->IsEnvironmentMapEnabled();
				if (ImGui::Checkbox("Environment Map", &_IsEnvironmentMapEnabled))
				{
					m_RendererLayer->SetEnabledEnvironmentMap(_IsEnvironmentMapEnabled);
				}


				static bool _IsEnabledHDR = m_RendererLayer->IsEnabledHDR();
				if (ImGui::Checkbox("HDR", &_IsEnabledHDR))
				{
					m_RendererLayer->SetHDR(_IsEnabledHDR);
				}
				

				static float3 _EnvironmentMapMultiplier = m_RendererLayer->GetEnvironmentMapMultiplier();
				if (ImGui::ColorEdit3("Environment Map Multiplier", &_EnvironmentMapMultiplier.x, ImGuiColorEditFlags_Float))
				{
					m_RendererLayer->SetEnvironmentMapMultiplier(_EnvironmentMapMultiplier);
				}


				static float3 s_AmbientLightStrength = m_RendererLayer->GetAmbientLightStrength();
				if (ImGui::ColorEdit3("Global Ambient Light", &s_AmbientLightStrength.x, ImGuiColorEditFlags_Float))
				{
					m_RendererLayer->SetAmbientLightStrength(s_AmbientLightStrength);
				}


				ImGui::End();
			}

			//Render scene Hierarchy Panel
			{
				ImGui::Begin("Scene Panel");

				m_SceneHierarchyPanel.OnImGuiRender();

				ImGui::End();
			}//End of Render scene Hierarchy Panel


			//Render Property Panel
			{
				ImGui::Begin("Properties");

				m_EntityPropertyPanel.SetContext(GetActiveScene()->GetSelectedEntity());
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
						switch (Settings::GetRendererAPI())
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
						ImGui::TextColored(ImVec4{ 0.529, 0.952, 0.486 ,1.0f }, "%.3f ms", m_Timer.GetAverageCpuTime() * 1000);
						ImGui::Text("RenderPass Time: %0.3f ms", m_TimerAvg_UpdateRenderPasses.GetAverageTime());
						ImGui::Text("ImGUiPass Time: %0.3f ms", m_TimerAvg_ImGuiPass.GetAverageTime());
						ImGui::Text("AppUpdate Time: %0.3f ms", m_TimerAvg_Update.GetAverageTime());

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
		static bool _SelectedModel = false;
		static bool _ImportModel = false;
		static char _ModelName[50] = "Model_";

		if (imguiLayer->CheckFileDialog(&fileBrowserImportModel, importFilePath))
		{
			_SelectedModel = true;
			
		}
		if (_SelectedModel)
		{
			ImGui::OpenPopup("AskModelNamePopUp");
		}

		if (ImGui::BeginPopup("AskModelNamePopUp"))
		{
			ImGui::Text("Model Name:");
			ImGui::InputText("", _ModelName, sizeof(_ModelName));
			if (ImGui::Button("OK"))
			{
				_ImportModel = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (_ImportModel)
		{
			GetActiveScene()->ImportModel(importFilePath.c_str(), nullptr);
			strcpy_s(_ModelName, "Model_");
			_SelectedModel = false;
			_ImportModel = false;
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

		/*auto onEntityTransform = [this](Event& event)
		{
			EventEntityTransform& _e = static_cast<EventEntityTransform&>(event);

			Entity _entity = _e.GetEntity();
			const BoundingAABox& _BoundingBox = _entity.GetComponent<BoundingBoxComponent>().GetBoundingBox();
			BoundingAABox _TransformedBoundingBox = Math::TransformBoundingBox(_BoundingBox, _entity.GetComponent<TransformComponent>().GetTransform());

			GetActiveScene()->UpdateBoundingBox(_TransformedBoundingBox);
		};

		RegisterEventListener(EventType::EntityTransform, onEntityTransform);*/
		
	}

}
