/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef CHECKBOXBASE_HPP
#define CHECKBOXBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/RadioButton.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/RadioButtonGroup.hpp>

class CheckBoxBase : public touchgfx::Container
{
public:
    CheckBoxBase();
    virtual ~CheckBoxBase() {}
    virtual void initialize();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::RadioButton check;
    touchgfx::TextAreaWithOneWildcard text;

private:

};

#endif // CHECKBOXBASE_HPP
