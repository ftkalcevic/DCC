#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <gui/dccconfig_screen/DCCConfigPresenter.hpp>

DCCConfigPresenter::DCCConfigPresenter(DCCConfigView& v)
    : view(v)
{

}

void DCCConfigPresenter::activate()
{

}

void DCCConfigPresenter::deactivate()
{
	EnableProgTrack(false);
}


void DCCConfigPresenter::UIMessage( UIMsg &msg ) 
{
	switch (msg.type)
	{
		case EUIMessageType::ScanTrackReply:
			view.ScanTrackReply(msg.scan.address, msg.scan.config, msg.scan.extendedAddress, msg.scan.manufacturer, msg.scan.version);
			break;
		default:
			break;
	}
}
	
void DCCConfigPresenter::EnableProgTrack(bool enable)
{
	model->EnableProgTrack(enable);
}

void DCCConfigPresenter::ScanProgrammingTrack()
{
	model->ScanProgrammingTrack();
}
