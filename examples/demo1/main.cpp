#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Window.hpp>

#include "../common/UnityToolBar.hpp"
#include "../common/HierarchyWidget.hpp"
#include "../common/DirTreeWidget.hpp"
#include "../common/FileListWidget.hpp"

using namespace FishGUI;



class IMWidget2 : public IMWidget
{
public:
	IMWidget2(const char* name) : IMWidget(name)
	{
	}
	
	virtual void DrawImpl() override
	{
		if (m_renderFunction != nullptr)
		{
			m_renderFunction();
		}
	}
	
	typedef std::function<void(void)> RenderFunction;

	void SetRenderFunction(RenderFunction func)
	{
		m_renderFunction = func;
	}
	
protected:
	RenderFunction	m_renderFunction;
};

int main()
{
//	Theme theme;
	FishGUI::Init();
	
	auto win = dynamic_cast<MainWindow*>( WindowManager::GetInstance().GetMainWindow() );

	auto right = new TabWidget("right");
	right->SetWidth(270);
	right->SetMinSize(250, 150);
	auto inspector = new IMWidget2("Inspector");
	right->AddChild(inspector);

	auto bottom = new TabWidget("bottom");
	bottom->SetHeight(180);
	bottom->SetMinSize(150, 150);
	auto project = new Widget("Project");
	auto console = new IMWidget2("Console");
	bottom->AddChild(project);
	bottom->AddChild(console);
	
//	auto rootNode = new FileNode("/Users/yushroom/program/FishEngine/Example/UnityChan-crs/Assets");
	auto rootNode = new FileNode("/Users/yushroom/program/FishEngine/Example/PBR/Assets");
//	rootNode->Find("/Users/yushroom/program/FishEngine/Example/Sponza/Assets/texture");
	auto dirs = new DirTreeWidget("Dirs", rootNode);
	dirs->SetWidth(150);
	dirs->SetMinSize(100, 100);
	auto files = new FileListWidget("Files");
	files->SetWidth(400);
	files->SetMinSize(200, 100);
//	dirs->SetRoot(rootNode);
	files->SetRoot(rootNode);
	dirs->GetSelectionModel()->SetSelectionChangedCallback([files](FileNode* node){
		files->SetRoot(node);
	});
	{
		auto layout = new SplitLayout(Orientation::Horizontal);
		project->SetLayout(layout);
		layout->Split(dirs, files);
	}

	auto left = new TabWidget("left");
	left->SetWidth(200);
	left->SetMinSize(200, 150);
	auto hierarchy = new HierarchyWidget("Hierarchy");
	left->AddChild(hierarchy);

	auto center = new TabWidget("center");
	center->SetWidth(500);
	center->SetMinSize(150, 150);
	auto scene = new IMWidget2("Scene");
	auto game = new IMWidget2("Game");
	auto assetStore = new IMWidget2("Asset Store");
	center->AddChild(scene);
	center->AddChild(game);
	center->AddChild(assetStore);

	auto layout1 = new SplitLayout(Orientation::Horizontal);
	auto layout2 = new SplitLayout(Orientation::Vertical);
	auto layout3 = new SplitLayout(Orientation::Horizontal);

	win->SetLayout(layout1);
	layout1->Split(layout2, right);
	layout2->Split(layout3, bottom);
	layout3->Split(left, center);
	
	std::string email = "thisisaverylongmail@example.com";
	bool enabled = true;
	FishGUI::Vector3f position{0, 1, -10};
	FishGUI::Vector3f rotation{0, 0, 0};
	FishGUI::Vector3f scale{1, 1, 1};
	
	win->SetToolBar(new UnityToolBar());
	win->SetStatusBar(new StatusBar());
	
	float fov = 60;
//	float near = 0.3f;
//	float far = 1000.f;
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
		FishGUI::Combox("Clear Flags", "Skybox");
		FishGUI::Combox("Culling Mask", "Everything");
		FishGUI::Combox("Projection", "Perspective");
		FishGUI::Slider("Field of View", fov, 1, 179);
		if (FishGUI::CheckBox("Allow HDR", allowHDR))
		{
			printf("value of [Allow HDR] is changed\n");
		}
		FishGUI::CheckBox("Allow MSAA", allowMSAA);
		FishGUI::CheckBox("Allow Dynamic Resolution", allowDynamicResolution);
		FishGUI::EndGroup();
		
//		FishGUI::InputText("Email", email);
//		float value = 0.4f;
//		FishGUI::Slider("Diameter", value, 0.0f, 1.0f);
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
	
//	project->SetRenderFunction(f1);
//	dirs->SetRenderFunction(f1);
//	files->SetRenderFunction(f2);
	inspector->SetRenderFunction(f1);
	scene->SetRenderFunction(f2);
	game->SetRenderFunction(f1);
	

//	{
//		auto win2 = FishGUI::NewWindow("dialog");
//		auto t = new TabWidget("center");
//		auto d = new IMWidget2("dialog");
//		t->AddChild(d);
//		auto layout = new SimpleLayout();
//		layout->SetWidget(t);
//		win2->SetLayout(layout);
//		d->SetRenderFunction(f1);
//	}

	FishGUI::Run();
	exit(EXIT_SUCCESS);
}
