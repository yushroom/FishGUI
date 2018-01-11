#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>

#include "FishGUI.hpp"
#include "Window.hpp"

using namespace FishGUI;

class UnityToolBar : public ToolBar
{
public:
	UnityToolBar() : ToolBar()
	{
	}
	
	UnityToolBar(UnityToolBar&) = delete;
	UnityToolBar& operator=(UnityToolBar&) = delete;
	
	virtual void Draw() override
	{
		// left
		constexpr int x_margin = 10;
		constexpr int y_margin = 4;
		//		constexpr int x_padding = 2;
		constexpr int leftButtonWidth = 32;
		auto rect = m_rect;
		auto r = rect;
		r.x += x_margin;
		r.y += y_margin;
		r.height -= y_margin*2;
		
//		const char* tools[] = {"H", "T", "R", "S"};
		int icons[] = {0xe900, 0xe902, 0xe984, 0xe989};
		r.width = leftButtonWidth * 4;
		SegmentedButtons(4, nullptr, icons, r);
		const int anchor = r.x+r.width;
		
		// middle
		{
			const int mid = (rect.x + rect.width)/2;
			constexpr int buttonWidth = 32;
			r.width = buttonWidth * 3;
			r.x = mid - buttonWidth*3/2;
//			const char* labels[] = {"Start", "Pause", "Next"};
			int icons[] = {0xe903, 0xe904, 0xe906};
			SegmentedButtons(3, nullptr, icons, r);
		}
		
		//
		{
			constexpr int buttonWidth = 64;
			r.x = anchor + 20;
			r.y += 2;
			r.height -= 2*2;
			r.width = buttonWidth * 2;
			const char* labels[] = {"Pivot", "Global"};
			int icons[] = {0xea1c, 0xea1d};
			SegmentedButtons(2, labels, icons, r);
		}
		
		// right
		{
			constexpr int buttonWidth = 78;
			r.x = rect.x + rect.width - x_margin - buttonWidth;
			r.width = buttonWidth;
			Button("Layout", r);
			
			r.x -= 12 + buttonWidth;
			Button("Layers", r);
		}
	}
};

class HierarchyWidget : public IMWidget
{
public:
	
	HierarchyWidget(const char* name)
		: IMWidget(name)
	{
	}
	
	void __Cell(const std::string& name)
	{
		auto rect = NewLine(16);
		
//		auto ctx = m_context->m_nvgContext;
//		nvgFontSize(ctx, 30);
//		nvgFontFace(ctx, "icons");
//		nvgFillColor(ctx, theme.mTextColor);
//		nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
//		int x = rect.x + 30/2 + 2;
//		int y = rect.y + 16/2;
//		nvgText(ctx, x, y, "▾", nullptr);
		
		rect.x += 20+2;
		rect.width -= 20 + 2;
		Label(name, rect);
	}
	
	virtual void DrawImpl() override
	{
		Indent(10);
		__Cell("GameObject" + std::to_string(0));
		for (int i = 1; i < 10; ++i)
		{
			Indent(10);
			__Cell("GameObject" + std::to_string(i));
		}
		for (int i = 1; i < 10; ++i)
		{
			Unindent(10);
		}
		
		for (int i = 10; i < 20; ++i)
		{
			__Cell("GameObject" + std::to_string(i));
		}
		Unindent(10);
	}
};

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
	auto project = new IMWidget2("Project");
	auto console = new IMWidget2("Console");
	bottom->AddChild(project);
	bottom->AddChild(console);

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
		FishGUI::Combox("Clear Flags", "Skybox");
		FishGUI::Combox("Culling Mask", "Everything");
		FishGUI::Combox("Projection", "Perspective");
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
	scene->SetRenderFunction(f2);
	game->SetRenderFunction(f1);
	

	{
		auto win2 = FishGUI::NewWindow("dialog");
		auto t = new TabWidget("center");
		auto d = new IMWidget2("dialog");
		t->AddChild(d);
		auto layout = new SimpleLayout();
		layout->SetWidget(t);
		win2->SetLayout(layout);
		d->SetRenderFunction(f1);
	}

	FishGUI::Run();
	exit(EXIT_SUCCESS);
}
