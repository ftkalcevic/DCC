/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef DECODERSVIEWBASE_HPP
#define DECODERSVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/decoders_screen/DecodersPresenter.hpp>
#include <touchgfx/widgets/TiledImage.hpp>
#include <gui/containers/StatusBar.hpp>
#include <gui/containers/FunctionButtons.hpp>
#include <touchgfx/containers/scrollers/ScrollWheel.hpp>
#include <gui/containers/Decoder.hpp>

class DecodersViewBase : public touchgfx::View<DecodersPresenter>
{
public:
    DecodersViewBase();
    virtual ~DecodersViewBase() {}
    virtual void setupScreen();

    virtual void scrollWheelDecodersUpdateItem(Decoder& item, int16_t itemIndex)
    {
        // Override and implement this function in Decoders
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::TiledImage backgroundImage;
    StatusBar statusBar;
    FunctionButtons functionButtons;
    touchgfx::ScrollWheel scrollWheelDecoders;
    touchgfx::DrawableListItems<Decoder, 3> scrollWheelDecodersListItems;
    touchgfx::TiledImage tiledImage1;

private:
    touchgfx::Callback<DecodersViewBase, touchgfx::DrawableListItemsInterface*, int16_t, int16_t> updateItemCallback;
    void updateItemCallbackHandler(touchgfx::DrawableListItemsInterface* items, int16_t containerIndex, int16_t itemIndex);

};

#endif // DECODERSVIEWBASE_HPP
