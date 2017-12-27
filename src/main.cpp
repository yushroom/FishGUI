#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>

#include "FishGUI.hpp"
#include "Window.hpp"

using namespace FishGUI;

int main()
{
	FishGUI::Init();
	auto win = WindowManager::GetInstance().GetMainWindow();
	auto ctx = win->context();

	auto right = new TabWidget(ctx, "right");
	right->SetWidth(160);
	right->SetMinSize(150, 150);
	auto inspector = new IMWidget(ctx, "Inspector");
	right->AddChild(inspector);

	auto bottom = new TabWidget(ctx, "bottom");
	bottom->SetHeight(180);
	bottom->SetMinSize(150, 150);
	auto project = new IMWidget(ctx, "Project");
	auto console = new IMWidget(ctx, "Console");
	bottom->AddChild(project);
	bottom->AddChild(console);

	auto left = new TabWidget(ctx, "left");
	left->SetWidth(140);
	left->SetMinSize(150, 150);
	auto hierarchy = new IMWidget(ctx, "Hierarchy");
	left->AddChild(hierarchy);

	auto center = new TabWidget(ctx, "center");
	center->SetWidth(500);
	center->SetMinSize(150, 150);
	auto scene = new IMWidget(ctx, "Scene");
	auto game = new IMWidget(ctx, "Game");
	auto assetStore = new IMWidget(ctx, "Asset Store");
	center->AddChild(scene);
	center->AddChild(game);
	center->AddChild(assetStore);

	auto layout1 = new SplitLayout(ctx, Orientation::Horizontal);
	auto layout2 = new SplitLayout(ctx, Orientation::Vertical);
	auto layout3 = new SplitLayout(ctx, Orientation::Horizontal);

	win->SetLayout(layout1);
	layout1->Split(layout2, right);
	layout2->Split(layout3, bottom);
	layout3->Split(left, center);

	FishGUI::Run();
	exit(EXIT_SUCCESS);
}
