#include <gui/containers/FunctionButtons.hpp>

FunctionButtons::FunctionButtons()
{
	textArea1.setWildcard((uint16_t *)u"Fn1");
	textArea2.setWildcard((uint16_t *)u"Fn2");
	textArea3.setWildcard((uint16_t *)u"Fn3");
	textArea4.setWildcard((uint16_t *)u"Fn4");
	textArea5.setWildcard((uint16_t *)u"Bell");
}

void FunctionButtons::initialize()
{
    FunctionButtonsBase::initialize();
}
