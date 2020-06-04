/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/startupscreen_screen/StartupScreenViewBase.hpp>

StartupScreenViewBase::StartupScreenViewBase() :
    splashScreenCounter(0)
{

}

void StartupScreenViewBase::setupScreen()
{

}

//Handles delays
void StartupScreenViewBase::handleTickEvent()
{
    if(splashScreenCounter > 0)
    {
        splashScreenCounter--;
        if(splashScreenCounter == 0)
        {
            //GoToMain
            //When SplashScreen completed change screen to Main
            //Go to Main with screen transition towards South
            application().gotoMainScreenCoverTransitionSouth();
        }
    }
}

//Called when the screen is done with transition/load
void StartupScreenViewBase::afterTransition()
{
    //SplashScreen
    //When screen is entered delay
    //Delay for 1000 ms (60 Ticks)
    splashScreenCounter = SPLASHSCREEN_DURATION;
}
