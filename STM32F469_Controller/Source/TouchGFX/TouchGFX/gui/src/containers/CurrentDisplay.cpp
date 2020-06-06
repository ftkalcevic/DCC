#include <gui/containers/CurrentDisplay.hpp>
#include <touchgfx/Color.hpp>

CurrentDisplay::CurrentDisplay()
{
	graph.setWidth(getWidth());
	graph.setHeight(getHeight());
	add(graph);
}

void CurrentDisplay::initialize()
{
    CurrentDisplayBase::initialize();
}


void CurrentDisplay::Update(uint16_t main, uint16_t prog)
{
	graph.Update(main,prog);
}

