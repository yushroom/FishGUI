#define _CRT_SECURE_NO_WARNINGS
#include <FishGUI/GLEnvironment.hpp>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>

#include <FishGUI/FishGUI.hpp>
#include <FishGUI/Window.hpp>

#include "../common/UnityToolBar.hpp"
#include "../common/HierarchyView.hpp"
#include "../common/DirTreeWidget.hpp"
#include "../common/FileListWidget.hpp"
#include "../common/SceneViewWidget.hpp"
#include "../common/UnityLayout.hpp"

//#include "../common/StringListWidget.hpp"

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


std::string email = "thisisaverylongmail@example.com";
bool enabled = true;
FishGUI::Vector3f position{0, 1, -10};
FishGUI::Vector3f rotation{0, 0, 0};
FishGUI::Vector3f scale{1, 1, 1};

float fov = 60;
//	float near = 0.3f;
//	float far = 1000.f;
bool allowHDR = true;
bool allowMSAA = true;
bool allowDynamicResolution = false;


Window* GetMainWindow()
{
	auto win = NewWindow("Fish GUI", 950, 600);
	auto mainLayout = new UnityLayout();
	win->SetLayout(mainLayout);
	
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
	auto rootNode = new FileNode(ApplicationFilePath());
//	auto rootNode = new FileNode(R"(D:\program\FishGUI)");
//	rootNode->Find("/Users/yushroom/program/FishEngine/Example/Sponza/Assets/texture");
	auto dirs = new DirTreeWidget("Dirs", rootNode);
	dirs->SetWidth(150);
	dirs->SetMinSize(100, 100);
	auto files = new UnityFileWidget("Files");
	files->SetWidth(400);
	files->SetMinSize(200, 100);
	files->GetFileListWidget()->SetRoot(rootNode);
	dirs->GetSelectionModel()->OnSelectionChanged.connect([files](FileNode* node){
		if (node != nullptr)
			files->GetFileListWidget()->SetRoot(node);
	});
	
	{
		auto layout = new SplitLayout(Orientation::Horizontal);
		project->SetLayout(layout);
		layout->Split(dirs, files);
	}
	
	auto left = new TabWidget("left");
	left->SetWidth(200);
	left->SetMinSize(200, 150);
	auto hierarchy = new HierarchyView("Hierarchy");
	left->AddChild(hierarchy);
	
	auto center = new TabWidget("center");
	center->SetWidth(500);
	center->SetMinSize(150, 150);
	auto scene = new SceneViewWidget("Scene");
	auto game = new IMWidget2("Game");
	auto assetStore = new IMWidget2("Asset Store");
	center->AddChild(scene);
	center->AddChild(game);
	center->AddChild(assetStore);
	
	auto layout1 = new SplitLayout(Orientation::Horizontal);
	auto layout2 = new SplitLayout(Orientation::Vertical);
	auto layout3 = new SplitLayout(Orientation::Horizontal);
	
	mainLayout->SetCenterLayout(layout1);
	layout1->Split(layout2, right);
	layout2->Split(layout3, bottom);
	layout3->Split(left, center);
	
	auto toolBar = new UnityToolBar();
	mainLayout->SetToolBar(toolBar);
	auto statusBar = new StatusBar();
	mainLayout->SetStatusBar(statusBar);
	
	auto f1 = [statusBar](){
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
//			printf("value of [Allow HDR] is changed\n");
			statusBar->SetText("value of[Allow HDR] is changed");
		}
		FishGUI::CheckBox("Allow MSAA", allowMSAA);
		FishGUI::CheckBox("Allow Dynamic Resolution", allowDynamicResolution);
		FishGUI::EndGroup();
		
		FishGUI::Group("Test");
		FishGUI::InputText("Email", email);
		//float value = 0.4f;
		//FishGUI::Slider("Diameter", value, 0.0f, 1.0f);
		FishGUI::EndGroup();
		for (int i = 0; i < 20; ++i) {
			std::string text = "button " + std::to_string(i);
			if (FishGUI::Button(text)) {
//				printf("%s clicked\n", text.c_str());
				std::ostringstream sout;
				sout << "button " << i << " clicked";
				statusBar->SetText(sout.str());
			}
		}
	};
	
	inspector->SetRenderFunction(f1);
	game->SetRenderFunction(f1);
	scene->SetIsFocused(true);
	win->SetPreDraw([scene]() {
		scene->RenderScene();
	});
	
	toolBar->OnRun.connect([scene]() {
		scene->Run();
	});
	
	toolBar->OnStop.connect([scene]() {
		scene->Stop();
	});
	
	toolBar->OnPause.connect([scene]() {
		scene->Pause();
	});
	
	toolBar->OnResume.connect([scene]() {
		scene->Resume();
	});
	
	toolBar->OnNextFrame.connect([scene]() {
		scene->NextFrame();
	});
	
	return win;
}


int main()
{
	FishGUI::Init();
	auto dialog1 = NewWindow("Open Dialog", 600, 400);
	
#if 0
	{
//		auto win2 = FishGUI::NewWindow("Dialog", 400, 400);
		auto t = new TabWidget("center");
		auto d = new IMWidget2("Dialog");
		t->AddChild(d);
		auto layout = new SimpleLayout();
		layout->SetWidget(t);
		dialog1->SetLayout(layout);
		d->SetRenderFunction(f1);
	}
	{
		auto win2 = FishGUI::NewWindow("Dialog2", 400, 400);
		auto t = new TabWidget("center");
		auto d = new IMWidget2("Dialog");
		t->AddChild(d);
		auto layout = new SimpleLayout();
		layout->SetWidget(t);
		win2->SetLayout(layout);
		d->SetRenderFunction(f1);
	}
#else
	
	std::string name = "New Project";
	std::string path = "/Users/yushroom/program/FishEngine/Example";
	
	
	auto f2 = [&name, &path]() {
//		FishGUI::Label("Project name*");
		FishGUI::InputText("Project name*", name);
//		FishGUI::Label("Location*");
		FishGUI::InputText("Location*", path);
		if (FishGUI::Button("Open an existing project")) {
		}
		if (FishGUI::Button("Cancel")) {
		}
		if (FishGUI::Button("Create project")) {
		}
	};
	
//		auto slw = new StringListWidget("Projects");
//		std::vector<std::string> projects = {"Empty", "Illustrative-Rendering", "PBR"};
//		for (auto& p : projects)
//		{
//			slw->AddItem(p);
//		}
	auto rootNode = new FileNode(path);
	auto dirs = new DirTreeWidget("Dirs", rootNode);

	auto tw = new TabWidget("center");
	auto tab1 = new IMWidget2("Projects");
	auto tab2 = new IMWidget2("Open New");
	tw->AddChild(dirs);
	tw->AddChild(tab2);
	auto layout = new SimpleLayout();
	layout->SetWidget(tw);
	dialog1->SetLayout(layout);
	tab2->SetRenderFunction(f2);
	dirs->GetSelectionModel()->OnSelectionChanged.connect([dialog1](FileNode* node){
		if (node != nullptr)
		{
			glfwSetWindowShouldClose(dialog1->GetGLFWWindow(), GLFW_TRUE);
			GetMainWindow();
		}
	});
#endif

	FishGUI::Run();
	exit(EXIT_SUCCESS);
}
