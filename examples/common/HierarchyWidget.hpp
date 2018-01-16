#pragma once

#include <vector>
#include <set>
#include <FishGUI/Widget.hpp>

#include "TSelectionModel.hpp"
#include "ItemModel.hpp"

#include "TreeViewWidget.hpp"

struct GameObject
{
	std::string name;
	std::vector<GameObject*> children;
	
	GameObject(const std::string& name) : name(name) {}
};

typedef TTreeModel<GameObject*, std::string> HierarchyModel;

template<>
inline GameObject* HierarchyModel::childAt(GameObject* parent, int row) const
{
	return parent->children[row];
}

template<>
inline int HierarchyModel::rowCount(GameObject* parent) const
{
	if (parent == nullptr)
		return 0;
	return parent->children.size();
}

template<>
inline std::string HierarchyModel::data(GameObject* item) const
{
	return item->name;
}



class HierarchyWidget : public FishGUI::TreeWidget<GameObject*>
{
public:
	typedef FishGUI::TreeWidget<GameObject*> Super;
	
	HierarchyWidget(const char* name);
	
private:
};
