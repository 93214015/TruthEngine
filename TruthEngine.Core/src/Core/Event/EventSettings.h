#pragma once
#include "Event.h"

namespace TruthEngine
{


	class EventSettingsGraphicsHDR : public Event
	{
		EVENT_CLASS_CATEGORY(EventCategorySettings);
		EVENT_CLASS_TYPE(SettingsGraphicsHDR);
	};

	class EventSettingsGraphicsFrameLimit : public Event
	{
		EVENT_CLASS_CATEGORY(EventCategorySettings);
		EVENT_CLASS_TYPE(SettingsGraphicsFrameLimit);
	};
}
