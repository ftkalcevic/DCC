#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "UIMessage.h"


Model::Model() : modelListener(0)
{
}

void Model::tick()
{
	while (uimsg.QueueLength() > 0)
	{
		UIMsg msg;
		if (uimsg.Receive(msg))
		{
			switch (msg.type)
			{
				case MainTrackStatusUpdate:
					mainTrackStatus = msg.hbStatus;
					if (modelListener)
						modelListener->TrackStatusChanged();
					break;
				case ProgrammingTrackStatusUpdate:
					progTrackStatus = msg.hbStatus;
					if (modelListener)
						modelListener->TrackStatusChanged();
					break;
				default:
					break;
			}
		}
	}
}

