#include "HierarchyWidget.hpp"
#include <FishGUI/Draw.hpp>
#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Theme.hpp>
#include <FishGUI/Input.hpp>

//#include <algorithm>

using namespace FishGUI;

HierarchyWidget::HierarchyWidget(const char* name) : Super(name)
{
	m_selectionModel.SetSelectionType( SelectionType::Multi );
	m_imContext->ymargin = 0;
	m_root = new GameObject("Root");
	m_root->children.push_back(new GameObject("Main Camera"));
	m_root->children.push_back(new GameObject("Directional Light"));
	auto go = new GameObject("GameObject");
	m_root->children.push_back(go);
	for (int i = 0; i < 30; ++i)
	{
		go->children.push_back(new GameObject("GameObject" + std::to_string(i)));
	}
	m_unfolded.insert(m_root);
	m_unfolded.insert(go);
//	m_model.UnfoldItem(m_root);
//	m_model.UnfoldItem(go);
	
	for (int i = 0; i < 10; ++i)
	{
		go = go->children[0];
		go->children.push_back(new GameObject("Child" + std::to_string(i)));
	}
	
	m_model.SetRootItem(m_root);
	m_selectionModel.Select(m_root);
}


/*
void HierarchyWidget::Cell(GameObject* go, MouseEvent* e)
{
	m_selectionModel.m_visiableItems.push_back(go);
	auto it2 = m_unfolded.find(go);
	bool isUnfolded = (it2 != m_unfolded.end());
//	bool isUnfolded = !m_model.IsItemFolded(go);
	
	bool outOfRange;
	auto rect = m_imContext->NextCell(IMGUIContext::cellHeight, outOfRange);
	if (!outOfRange)
	{
		Rect preiconRect = rect;
		preiconRect.width = 20;
		
		auto ctx = Context::GetInstance().m_drawContext->vg;
		auto theme = Context::GetInstance().m_drawContext->theme;
		
		auto totalCellRect = rect;
		totalCellRect.x = m_rect.x;
		totalCellRect.width = m_rect.width;
		
		bool insidePreicon = PointInRect(e->pos, preiconRect);
		bool inside = (!insidePreicon) && PointInRect(e->pos, totalCellRect);
		bool clicked = inside && e->state == MouseButtonState::Up;
		bool clickedPreicon = false;
		
		if (clicked)
		{
			m_selectionModel.OnItemClicked(go, e);
		}
		
		bool selected = (m_selectionModel.m_selected.find(go) != m_selectionModel.m_selected.end());
		if (selected)
		{
			DrawRect(Context::GetInstance().m_drawContext, totalCellRect, nvgRGB(62, 125, 231));
		}
		
		// preicon
		int x = rect.x + 30/2 + 2;
		int y = rect.y + 16/2;
		if (!go->children.empty())
		{
			clickedPreicon = insidePreicon && e->state == MouseButtonState::Up;
			if (clickedPreicon)
			{
				if (isUnfolded)
					m_unfolded.erase(it2);
//					m_model.FoldItem(go);
				else
					m_unfolded.insert(go);
//					m_model.UnfoldItem(go);
				e->isAccepted = true;
			}
			
//			auto icon = m_model.icon(go);
			nvgFontSize(ctx, 32);
			nvgFontFace(ctx, "icons");
			nvgFillColor(ctx, theme->textColor);
			nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			if (isUnfolded)
				nvgText(ctx, x, y, "▾", nullptr);
			else
				nvgText(ctx, x, y, "▸", nullptr);
//			nvgText(ctx, x, y, icon->fontText.c_str(), nullptr);
		}
		
		// text
		rect.x += 20+2;
		rect.width -= 20 + 2;
		Label(m_model.data(go), rect);
	}
	
	Indent(10);
	if (m_model.hasChildren(go) && isUnfolded)
	{
		for (int i = 0; i < m_model.rowCount(go); ++i)
		{
			Cell(m_model.childAt(go, i), e);
		}
	}
	Unindent(10);
}


void HierarchyWidget::DrawImpl()
{
	MouseEvent e;
	auto input = Context::GetInstance().m_input;
	bool inside = input->MouseInRect(m_rect);
	bool clicked = inside && input->GetMouseButtonUp(MouseButton::Left);
	if (clicked)
	{
		e.button = static_cast<int>(MouseButton::Left);
		e.pos = input->GetMousePosition();
		e.state = MouseButtonState::Up;
		e.isAccepted = false;
		e.modifiers = input->m_mouseEventModifiers[0];
	}
	else
	{
		// no mouse event, just set isAccepted = true
		e.isAccepted = true;
	}
	
	m_selectionModel.BeforeFrame();
	
	Indent(10);
	Cell(m_root, &e);
	Unindent(10);
	
	
	if (!e.isAccepted)
	{
		m_selectionModel.ClearSelections();
	}
	m_selectionModel.AfterFrame(&e);
}
*/
