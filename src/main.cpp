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
	auto ctx = win->GetContext();

	auto right = new TabWidget(ctx, "right");
	right->SetWidth(270);
	right->SetMinSize(250, 150);
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
	
	std::string email = "thisisaverylongmail@example.com";
	bool enabled = true;
	FishGUI::Vector3f position{0, 1, -10};
	FishGUI::Vector3f rotation{0, 0, 0};
	FishGUI::Vector3f scale{1, 1, 1};
	
	float fov = 60;
	float near = 0.3f;
	float far = 1000.f;
	bool allowHDR = true;
	bool allowMSAA = true;
	bool allowDynamicResolution = false;
	
	auto f1 = [&](){
		FishGUI::Group("Transform");
		FishGUI::Float3("Position", position.x, position.y, position.z);
		FishGUI::Float3("Rotation", rotation.x, rotation.y, rotation.z);
		FishGUI::Float3("Scale", scale.x, scale.y, scale.z);
		FishGUI::EndGroup();
		
		FishGUI::Group("Camera");
		FishGUI::Slider("Field of View", fov, 1, 179);
		FishGUI::CheckBox("Allow HDR", allowHDR);
		FishGUI::CheckBox("Allow MSAA", allowMSAA);
		FishGUI::CheckBox("Allow Dynamic Resolution", allowDynamicResolution);
		FishGUI::EndGroup();
		
		FishGUI::InputText("Email", email);
		float value = 0.4f;
		FishGUI::Slider("Diameter", value, 0.0f, 1.0f);
		for (int i = 0; i < 20; ++i) {
			std::string text = "button " + std::to_string(i);
			if (FishGUI::Button(text)) {
				printf("%s clicked\n", text.c_str());
			}
		}
	};
	
	auto f2 = [&]() {
		FishGUI::CheckBox("CheckBox", enabled);
		FishGUI::Label("Login");
		FishGUI::InputText("Email", email);
		FishGUI::Label("AaBbCcDdEeFfGg");
		if (FishGUI::Button("button 1")) {
			printf("button 1 clicked\n");
		}
		if (FishGUI::Button("button 2")) {
			printf("button 2 clicked\n");
		}
	};
	
	project->SetRenderFunction(f1);
	inspector->SetRenderFunction(f1);
	
	
	{
		auto win2 = FishGUI::NewWindow("dialog");
		auto t = new TabWidget(ctx, "center");
		auto d = new IMWidget(ctx, "dialog");
		t->AddChild(d);
		auto layout = new SimpleLayout();
		layout->SetWidget(t);
		win2->SetLayout(layout);
		d->SetRenderFunction(f2);
	}

	FishGUI::Run();
	exit(EXIT_SUCCESS);
}
