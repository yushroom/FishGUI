#pragma once

#include <FishGUI/ModelView/TreeWidget.hpp>

struct GameObject
{
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	
	GameObject(const std::string& name) : name(name) {}
	
	void AddChild(GameObject* child)
	{
		children.push_back(child);
		child->parent = this;
	}
};


class HierarchyModel : public FishGUI::TItemModel<GameObject*>
{
	typedef GameObject* T;
public:
	
	virtual T Parent(T child) const override
	{
		return child->parent;
	}
	
	virtual T ChildAt(int index, T parent) const override
	{
		return parent->children[index];
	}
	
	virtual int ChildCount(T item) const override
	{
		return (int)item->children.size();
	}

	virtual std::string Text(T item) const override
	{
		return item->name;
	}
};

class HierarchyView : public FishGUI::TreeWidget<GameObject*>
{
public:
	typedef FishGUI::TreeWidget<GameObject*> Super;

	HierarchyView(const char* name);
};
