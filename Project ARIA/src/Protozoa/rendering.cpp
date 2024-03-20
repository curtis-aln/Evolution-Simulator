#include "Protozoa.h"
#include "../Utils/utility.h"

void Protozoa::render()
{
	for (Cell& cell : m_cells)
	{
		cell.render(m_window, m_cell_renderer);
	}

	if (debug_mode)
	{
		render_debug();
	}
}


void Protozoa::render_debug()
{
	drawRectOutline(m_personal_bounds, *m_window, sf::RenderStates());

	//for (Cell& cell : m_cells)
	//{
	//	cell.render(m_window, m_cell_renderer);
	//}
}
