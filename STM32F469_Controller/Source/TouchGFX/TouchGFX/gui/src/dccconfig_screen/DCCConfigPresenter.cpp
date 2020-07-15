#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <gui/dccconfig_screen/DCCConfigPresenter.hpp>
#include "DecodersConfig.h"

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
	decoderDefinitions.clear();
	uiDecodersConfig.write();
}


void DCCConfigPresenter::UIMessage( UIMsg &msg ) 
{
	switch (msg.type)
	{
		case EUIMessageType::ScanTrackReply:
			view.ScanTrackReply(msg.scan.result, msg.scan.address, msg.scan.config, msg.scan.extendedAddress, msg.scan.manufacturer, msg.scan.version);
			break;
		case EUIMessageType::CVWriteReply:
			view.WriteReply(msg.result);
			break;
		case EUIMessageType::ScanAllCVsReply:
			view.ScanAllCVsReply(msg.scanAllCVs.result, msg.scanAllCVs.CV, msg.scanAllCVs.value);
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

void DCCConfigPresenter::ScanAllCVsTrack()
{
	model->ScanAllCVsTrack();
}

void DCCConfigPresenter::StopScanAllCVsTrack()
{
	model->StopScanAllCVsTrack();
}
