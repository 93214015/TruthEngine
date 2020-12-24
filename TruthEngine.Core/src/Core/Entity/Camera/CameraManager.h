#pragma once


namespace TruthEngine
{

	namespace Core
	{
		class ICamera;

		class CameraManager
		{
		public:
			void AddCamera(std::shared_ptr<ICamera> camera);

			std::shared_ptr<ICamera> GetCamera(std::string_view name);
			void SetActiveCamera(std::string_view name);

			static CameraManager* GetInstance();
		protected:


		protected:
			uint32_t CameraNum = 0;

			std::unordered_map<std::string_view, std::shared_ptr<ICamera>> m_Map_Cameras;
			std::shared_ptr<ICamera> m_ActiveCamera;
		};
	}
}
