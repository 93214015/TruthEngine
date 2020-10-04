#pragma once
#include "Core/Window.h"

namespace TruthEnigne::Win32 {

	class Win32Window : public TruthEngine::Core::Window {

	public:
		void OnUpdate() override;


		void SetEventCallBack(const EventCallBack& fn) override;


		std::unique_ptr<Window> Create() override;

	};

}