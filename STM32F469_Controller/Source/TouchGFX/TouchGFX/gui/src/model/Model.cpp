#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "ProgTrackDCC.h"
#include "MainTrackDCC.h"


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
				case EUIMessageType::MainTrackStatusUpdate:
					mainTrackStatus = msg.hbStatus;
					if (modelListener)
						modelListener->TrackStatusChanged();
					break;
				case EUIMessageType::ProgrammingTrackStatusUpdate:
					progTrackStatus = msg.hbStatus;
					if (modelListener)
						modelListener->TrackStatusChanged();
					break;
				default:
					modelListener->UIMessage(msg);
					break;
			}
		}
	}
}


int Model::getTripCurrent() 
{ 
	return 0; 
}
int Model::getToff() 
{ 
	return 0; 
}
int Model::getSlewRate() 
{ 
	return 0; 
}


void Model::EnableProgTrack(bool enable)
{
	ProgrammingTrack_DCC_Enable(enable);
}

void Model::ScanProgrammingTrack()
{
	ProgrammingTrack_DCC_ScanProgrammingTrack();
}

void Model::TakeControl(int decoderIndex, bool control)
{
	app.TakeControl(decoderIndex, control);
}

void Model::ScanAllCVsTrack()
{
	ProgrammingTrack_DCC_ScanAllCVs();
}
