#include <gui/settings_screen/SettingsView.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>

SettingsPresenter::SettingsPresenter(SettingsView& v)
    : view(v)
{

}

void SettingsPresenter::activate()
{
    
}

void SettingsPresenter::deactivate()
{

}


void SettingsPresenter::UIMessage( UIMsg &msg ) 
{
	switch (msg.type)
	{
		case EUIMessageType::InputEvent:
			view.UpdateInputs(msg.input.throttle, msg.input.brake, msg.input.throttleRaw, msg.input.brakeRaw, msg.input.direction);
			break;
	}
}
