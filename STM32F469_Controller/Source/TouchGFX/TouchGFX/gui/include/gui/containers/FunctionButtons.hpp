#ifndef FUNCTIONBUTTONS_HPP
#define FUNCTIONBUTTONS_HPP

#include <gui_generated/containers/FunctionButtonsBase.hpp>

class FunctionButtons : public FunctionButtonsBase
{
public:
    FunctionButtons();
    virtual ~FunctionButtons() {}

    virtual void initialize();
protected:
};

#endif // FUNCTIONBUTTONS_HPP
