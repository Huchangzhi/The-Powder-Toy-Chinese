#include "OptionsView.h"
#include "Format.h"
#include "OptionsController.h"
#include "OptionsModel.h"
#include "common/platform/Platform.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "gui/Style.h"
#include "simulation/ElementDefs.h"
#include "client/Client.h"
#include "gui/dialogues/ConfirmPrompt.h"
#include "gui/dialogues/InformationMessage.h"
#include "gui/interface/Button.h"
#include "gui/interface/Checkbox.h"
#include "gui/interface/DropDown.h"
#include "gui/interface/Engine.h"
#include "gui/interface/Label.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/DirectionSelector.h"
#include "PowderToySDL.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <SDL.h>

OptionsView::OptionsView():
	ui::Window(ui::Point(-1, -1), ui::Point(320, 340))
	{

	auto autowidth = [this](ui::Component *c) {
		c->Size.X = Size.X - c->Position.X - 12;
	};
	
	ui::Label * tempLabel = new ui::Label(ui::Point(4, 1), ui::Point(Size.X-8, 22), ByteString("设置").FromUtf8());
	tempLabel->SetTextColour(style::Colour::InformationTitle);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	autowidth(tempLabel);
	AddComponent(tempLabel);

	class Separator : public ui::Component
	{
		public:
		Separator(ui::Point position, ui::Point size) : Component(position, size){}
		virtual ~Separator(){}

		void Draw(const ui::Point& screenPos) override
		{
			GetGraphics()->BlendRect(RectSized(screenPos, Size), 0xFFFFFF_rgb .WithAlpha(180));
		}		
	};
	
	Separator *tmpSeparator = new Separator(ui::Point(0, 22), ui::Point(Size.X, 1));
	AddComponent(tmpSeparator);

	int currentY = 6;
	scrollPanel = new ui::ScrollPanel(ui::Point(1, 23), ui::Point(Size.X-2, Size.Y-39));
	
	AddComponent(scrollPanel);

	heatSimulation = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("热模拟 \bg34.0版本后加入").FromUtf8(), "");
	autowidth(heatSimulation);
	heatSimulation->SetActionCallback({ [this] { c->SetHeatSimulation(heatSimulation->GetChecked()); } });
	scrollPanel->AddChild(heatSimulation);
	currentY+=14;
	tempLabel = new ui::Label(ui::Point(24, currentY), ui::Point(1, 16), ByteString("\bg 关闭此选项可能导致一些奇怪的问题").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=16;
	ambientHeatSimulation = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("环境热模拟 \bg50.0版本后加入").FromUtf8(), "");
	autowidth(ambientHeatSimulation);
	ambientHeatSimulation->SetActionCallback({ [this] { c->SetAmbientHeatSimulation(ambientHeatSimulation->GetChecked()); } });
	scrollPanel->AddChild(ambientHeatSimulation);
	currentY+=14;
	tempLabel = new ui::Label(ui::Point(24, currentY), ui::Point(1, 16), ByteString("\bg 关闭此项可能导致一些沙盘不能正常运行").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=16;
	newtonianGravity = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("牛顿引力模拟 \bg48.0版本后加入").FromUtf8(), "");
	autowidth(newtonianGravity);
	newtonianGravity->SetActionCallback({ [this] { c->SetNewtonianGravity(newtonianGravity->GetChecked()); } });
	scrollPanel->AddChild(newtonianGravity);
	currentY+=14;
	tempLabel = new ui::Label(ui::Point(24, currentY), ui::Point(1, 16), ByteString("\bg 会降低游戏运行的效能").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=16;
	waterEqualisation = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("连通器模拟 \bg61.0版本后加入 ").FromUtf8(), "");
	autowidth(waterEqualisation);
	waterEqualisation->SetActionCallback({ [this] { c->SetWaterEqualisation(waterEqualisation->GetChecked()); } });
	scrollPanel->AddChild(waterEqualisation);
	currentY+=14;
	tempLabel = new ui::Label(ui::Point(24, currentY), ui::Point(1, 16), ByteString("\bg 有大量液体存在时会降低游戏运行的效能").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=19;
	airMode = new ui::DropDown(ui::Point(Size.X-95, currentY), ui::Point(80, 16));
	scrollPanel->AddChild(airMode);
	airMode->AddOption(std::pair<String, int>(ByteString("开启").FromUtf8(), 0));
	airMode->AddOption(std::pair<String, int>(ByteString("关闭压力").FromUtf8(), 1));
	airMode->AddOption(std::pair<String, int>(ByteString("关闭速度").FromUtf8(), 2));
	airMode->AddOption(std::pair<String, int>(ByteString("关闭").FromUtf8(), 3));
	airMode->AddOption(std::pair<String, int>(ByteString("更新停止").FromUtf8(), 4));
	airMode->SetActionCallback({ [this] { c->SetAirMode(airMode->GetOption().second); } });

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X-96, 16), ByteString("空气模拟模式").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	ambientAirTemp = new ui::Textbox(ui::Point(Size.X-95, currentY), ui::Point(60, 16));
	ambientAirTemp->SetActionCallback({ [this] {
		UpdateAirTemp(ambientAirTemp->GetText(), false);
	} });
	ambientAirTemp->SetDefocusCallback({ [this] {
		UpdateAirTemp(ambientAirTemp->GetText(), true);
	}});
	scrollPanel->AddChild(ambientAirTemp);

	ambientAirTempPreview = new ui::Button(ui::Point(Size.X-31, currentY), ui::Point(16, 16), "", ByteString("预览").FromUtf8());
	scrollPanel->AddChild(ambientAirTempPreview);

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X-96, 16), ByteString("环境空气温度").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	gravityMode = new ui::DropDown(ui::Point(Size.X-95, currentY), ui::Point(80, 16));
	scrollPanel->AddChild(gravityMode);
	gravityMode->AddOption(std::pair<String, int>(ByteString("竖直").FromUtf8(), 0));
	gravityMode->AddOption(std::pair<String, int>(ByteString("关闭").FromUtf8(), 1));
	gravityMode->AddOption(std::pair<String, int>(ByteString("中心").FromUtf8(), 2));
	gravityMode->AddOption(std::pair<String, int>(ByteString("自定义").FromUtf8(), 3));

	class GravityWindow : public ui::Window
	{
		void OnTryExit(ExitMethod method) override
		{
			CloseActiveWindow();
			SelfDestruct();
		}

		void OnDraw() override
		{
			Graphics * g = GetGraphics();

			g->DrawFilledRect(RectSized(Position - Vec2{ 1, 1 }, Size + Vec2{ 2, 2 }), 0x000000_rgb);
			g->DrawRect(RectSized(Position, Size), 0xC8C8C8_rgb);
		}

		ui::DirectionSelector * gravityDirection;
		ui::Label * labelValues;

		OptionsController * c;

	public:
		GravityWindow(ui::Point position, float scale, int radius, float x, float y, OptionsController * c_):
			ui::Window(position, ui::Point((radius * 5 / 2) + 20, (radius * 5 / 2) + 75)),
			gravityDirection(new ui::DirectionSelector(ui::Point(10, 32), scale, radius, radius / 4, 2, 5)),
			c(c_)
			{
				ui::Label * tempLabel = new ui::Label(ui::Point(4, 1), ui::Point(Size.X - 8, 22),  ByteString("自定义引力").FromUtf8());
				tempLabel->SetTextColour(style::Colour::InformationTitle);
				tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
				tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
				AddComponent(tempLabel);

				Separator * tempSeparator = new Separator(ui::Point(0, 22), ui::Point(Size.X, 1));
				AddComponent(tempSeparator);

				labelValues = new ui::Label(ui::Point(0, (radius * 5 / 2) + 37), ui::Point(Size.X, 16), String::Build(Format::Precision(1), "X:", x, " Y:", y, " Total:", std::hypot(x, y)));
				labelValues->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
				labelValues->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
				AddComponent(labelValues);

				gravityDirection->SetValues(x, y);
				gravityDirection->SetUpdateCallback([this](float x, float y) {
					labelValues->SetText(String::Build(Format::Precision(1), "X:", x, " Y:", y, " Total:", std::hypot(x, y)));
				});
				gravityDirection->SetSnapPoints(5, 5, 2);
				AddComponent(gravityDirection);

				ui::Button * okayButton = new ui::Button(ui::Point(0, Size.Y - 17), ui::Point(Size.X, 17), "OK");
				okayButton->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
				okayButton->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
				okayButton->Appearance.BorderInactive = ui::Colour(200, 200, 200);
				okayButton->SetActionCallback({ [this] {
					c->SetCustomGravityX(gravityDirection->GetXValue());
					c->SetCustomGravityY(gravityDirection->GetYValue());
					CloseActiveWindow();
					SelfDestruct();
				} });
				AddComponent(okayButton);
				SetOkayButton(okayButton);

				MakeActiveWindow();
			}
	};

	gravityMode->SetActionCallback({ [this] {
		c->SetGravityMode(gravityMode->GetOption().second);
		if (gravityMode->GetOption().second == 3)
			new GravityWindow(ui::Point(-1, -1), 0.05f, 40, customGravityX, customGravityY, c);
	} });

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X-96, 16), ByteString("引力模拟模式").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	edgeMode = new ui::DropDown(ui::Point(Size.X-95, currentY), ui::Point(80, 16));
	scrollPanel->AddChild(edgeMode);
	edgeMode->AddOption(std::pair<String, int>(ByteString("虚空").FromUtf8(), 0));
	edgeMode->AddOption(std::pair<String, int>(ByteString("固体").FromUtf8(), 1));
	edgeMode->AddOption(std::pair<String, int>(ByteString("循环").FromUtf8(), 2));
	edgeMode->SetActionCallback({ [this] { c->SetEdgeMode(edgeMode->GetOption().second); } });

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X-96, 16), ByteString("边界模式").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	temperatureScale = new ui::DropDown(ui::Point(Size.X-95, currentY), ui::Point(80, 16));
	scrollPanel->AddChild(temperatureScale);
	temperatureScale->AddOption(std::pair<String, int>(ByteString("开尔文").FromUtf8(), 0));
	temperatureScale->AddOption(std::pair<String, int>(ByteString("摄氏度").FromUtf8(), 1));
	temperatureScale->AddOption(std::pair<String, int>(ByteString("华氏度").FromUtf8(), 2));
	temperatureScale->SetActionCallback({ [this] { c->SetTemperatureScale(temperatureScale->GetOption().second); } });

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X-96, 16), ByteString("温度单位").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	tmpSeparator = new Separator(ui::Point(0, currentY), ui::Point(Size.X, 1));
	scrollPanel->AddChild(tmpSeparator);

	currentY+=4;
	scale = new ui::DropDown(ui::Point(8, currentY), ui::Point(40, 16));
	{
		int current_scale = ui::Engine::Ref().GetScale();
		int ix_scale = 1;
		bool current_scale_valid = false;
		do
		{
			if (current_scale == ix_scale)
				current_scale_valid = true;
			scale->AddOption(std::pair<String, int>(String::Build(ix_scale), ix_scale));
			ix_scale += 1;
		}
		while (desktopWidth >= GetGraphics()->Size().X * ix_scale && desktopHeight >= GetGraphics()->Size().Y * ix_scale);
		if (!current_scale_valid)
			scale->AddOption(std::pair<String, int>(ByteString("当前").FromUtf8(), current_scale));
	}
	scale->SetActionCallback({ [this] { c->SetScale(scale->GetOption().second); } });
	scrollPanel->AddChild(scale);

	tempLabel = new ui::Label(ui::Point(scale->Position.X+scale->Size.X+3, currentY), ui::Point(Size.X-40, 16), ByteString("\bg - 大屏幕的窗口比例因子").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	resizable = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("可调整大小").FromUtf8(), "");
	autowidth(resizable);
	resizable->SetActionCallback({ [this] { c->SetResizable(resizable->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(resizable->Position.X+Graphics::TextSize(resizable->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 允许调整大小和最大化窗口").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(resizable);

	currentY+=20;
	fullscreen = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("全屏").FromUtf8(), "");
	autowidth(fullscreen);
	fullscreen->SetActionCallback({ [this] { c->SetFullscreen(fullscreen->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(fullscreen->Position.X+Graphics::TextSize(fullscreen->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 进入全屏模式").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(fullscreen);

	currentY+=20;
	altFullscreen = new ui::Checkbox(ui::Point(23, currentY), ui::Point(1, 16), ByteString("更改分辨率").FromUtf8(), "");
	autowidth(altFullscreen);
	altFullscreen->SetActionCallback({ [this] { c->SetAltFullscreen(altFullscreen->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(altFullscreen->Position.X+Graphics::TextSize(altFullscreen->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 设置最佳屏幕分辨率").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(altFullscreen);

	currentY+=20;
	forceIntegerScaling = new ui::Checkbox(ui::Point(23, currentY), ui::Point(1, 16), ByteString("强制整数缩放").FromUtf8(), "");
	autowidth(forceIntegerScaling);
	forceIntegerScaling->SetActionCallback({ [this] { c->SetForceIntegerScaling(forceIntegerScaling->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(altFullscreen->Position.X+Graphics::TextSize(forceIntegerScaling->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 不再那么模糊").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(forceIntegerScaling);

	currentY+=20;
	fastquit = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("快速退出").FromUtf8(), "");
	autowidth(fastquit);
	fastquit->SetActionCallback({ [this] { c->SetFastQuit(fastquit->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(fastquit->Position.X+Graphics::TextSize(fastquit->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 点击关闭按钮时总是完全退出游戏").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(fastquit);

	currentY+=20;
	showAvatars = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("显示头像").FromUtf8(), "");
	autowidth(showAvatars);
	showAvatars->SetActionCallback({ [this] { c->SetShowAvatars(showAvatars->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(showAvatars->Position.X+Graphics::TextSize(showAvatars->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 禁用此项可减少网络带宽").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(showAvatars);

	currentY += 20;
	momentumScroll = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("加速/旧版滚动").FromUtf8(), "");
	autowidth(momentumScroll);
	momentumScroll->SetActionCallback({ [this] { c->SetMomentumScroll(momentumScroll->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(momentumScroll->Position.X + Graphics::TextSize(momentumScroll->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 启用此项时将步进滚动改为加速").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(momentumScroll);

	currentY+=20;
	mouseClickRequired = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("置顶类别").FromUtf8(), "");
	autowidth(mouseClickRequired);
	mouseClickRequired->SetActionCallback({ [this] { c->SetMouseClickrequired(mouseClickRequired->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(mouseClickRequired->Position.X+Graphics::TextSize(mouseClickRequired->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 启用此项时将滑动切换类别改为点击").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(mouseClickRequired);

	currentY+=20;
	includePressure = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("包括压力").FromUtf8(), "");
	autowidth(includePressure);
	includePressure->SetActionCallback({ [this] { c->SetIncludePressure(includePressure->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(includePressure->Position.X+Graphics::TextSize(includePressure->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 沙盘,复制,Stamps保存压力数据").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(includePressure);

	currentY+=20;
	perfectCirclePressure = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), ByteString("完美的圆").FromUtf8(), "");
	autowidth(perfectCirclePressure);
	perfectCirclePressure->SetActionCallback({ [this] { c->SetPerfectCircle(perfectCirclePressure->GetChecked()); } });
	tempLabel = new ui::Label(ui::Point(perfectCirclePressure->Position.X+Graphics::TextSize(perfectCirclePressure->GetText()).X+19, currentY), ui::Point(1, 16), ByteString("\bg - 由Notch创造的最完美的圆").FromUtf8());
	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(perfectCirclePressure);

	currentY+=20;
	decoSpace = new ui::DropDown(ui::Point(8, currentY), ui::Point(60, 16));
	decoSpace->SetActionCallback({ [this] { c->SetDecoSpace(decoSpace->GetOption().second); } });
	scrollPanel->AddChild(decoSpace);
	decoSpace->AddOption(std::pair<String, int>("sRGB", 0));
	decoSpace->AddOption(std::pair<String, int>("Linear", 1));
	decoSpace->AddOption(std::pair<String, int>("Gamma 2.2", 2));
	decoSpace->AddOption(std::pair<String, int>("Gamma 1.8", 3));

	tempLabel = new ui::Label(ui::Point(decoSpace->Position.X+decoSpace->Size.X+3, currentY), ui::Point(Size.X-40, 16), ByteString("\bg - 装饰工具使用的颜色空间").FromUtf8());
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	currentY+=20;
	ui::Button * dataFolderButton = new ui::Button(ui::Point(8, currentY), ui::Point(90, 16), ByteString("打开数据目录").FromUtf8());
	dataFolderButton->SetActionCallback({ [] {
		ByteString cwd = Platform::GetCwd();
		if (!cwd.empty())
			Platform::OpenURI(cwd);
		else
			fprintf(stderr, "cannot open data folder: Platform::GetCwd(...) failed\n");
	} });
	scrollPanel->AddChild(dataFolderButton);

	ui::Button * migrationButton = new ui::Button(ui::Point(Size.X - 178, currentY), ui::Point(163, 16), ByteString("迁移到共享数据目录").FromUtf8());
	migrationButton->SetActionCallback({ [] {
		ByteString from = Platform::originalCwd;
		ByteString to = Platform::sharedCwd;
		new ConfirmPrompt(ByteString("是否迁移").FromUtf8(), ByteString("\n所有所有Stamp、保存和脚本\n\bt->").FromUtf8() + from.FromUtf8() + ByteString("\bw\n迁移到共享数据目录\n\bt->").FromUtf8() + to.FromUtf8() + "\bw\n\n" +
			 ByteString("已存在的文件不会被覆盖").FromUtf8(), { [=] () {
				 String ret = Client::Ref().DoMigration(from, to);
				new InformationMessage(ByteString("迁移完成").FromUtf8(), ret, false);
			 } });
	} });
	scrollPanel->AddChild(migrationButton);

	ui::Button * tempButton = new ui::Button(ui::Point(0, Size.Y-16), ui::Point(Size.X, 16), "OK");
	tempButton->SetActionCallback({ [this] { c->Exit(); } });
	AddComponent(tempButton);
	SetCancelButton(tempButton);
	SetOkayButton(tempButton);
	currentY+=20;
	scrollPanel->InnerSize = ui::Point(Size.X, currentY);
}

void OptionsView::UpdateAmbientAirTempPreview(float airTemp, bool isValid)
{
	if (isValid)
	{
		ambientAirTempPreview->Appearance.BackgroundInactive = RGB<uint8_t>::Unpack(HeatToColour(airTemp)).WithAlpha(0xFF);
		ambientAirTempPreview->SetText("");
	}
	else
	{
		ambientAirTempPreview->Appearance.BackgroundInactive = ui::Colour(0, 0, 0);
		ambientAirTempPreview->SetText("?");
	}
	ambientAirTempPreview->Appearance.BackgroundHover = ambientAirTempPreview->Appearance.BackgroundInactive;
}

void OptionsView::AmbientAirTempToTextBox(float airTemp)
{
	StringBuilder sb;
	sb << Format::Precision(2);
	format::RenderTemperature(sb, airTemp, temperatureScale->GetOption().second);
	ambientAirTemp->SetText(sb.Build());
}

void OptionsView::UpdateAirTemp(String temp, bool isDefocus)
{
	// Parse air temp and determine validity
	float airTemp = 0;
	bool isValid;
	try
	{
		airTemp = format::StringToTemperature(temp, temperatureScale->GetOption().second);
		isValid = true;
	}
	catch (const std::exception &ex)
	{
		isValid = false;
	}

	// While defocusing, correct out of range temperatures and empty textboxes
	if (isDefocus)
	{
		if (temp.empty())
		{
			isValid = true;
			airTemp = float(R_TEMP) + 273.15f;
		}
		else if (!isValid)
			return;
		else if (airTemp < MIN_TEMP)
			airTemp = MIN_TEMP;
		else if (airTemp > MAX_TEMP)
			airTemp = MAX_TEMP;

		AmbientAirTempToTextBox(airTemp);
	}
	// Out of range temperatures are invalid, preview should go away
	else if (isValid && (airTemp < MIN_TEMP || airTemp > MAX_TEMP))
		isValid = false;

	// If valid, set temp
	if (isValid)
		c->SetAmbientAirTemperature(airTemp);

	UpdateAmbientAirTempPreview(airTemp, isValid);
}

void OptionsView::NotifySettingsChanged(OptionsModel * sender)
{
	temperatureScale->SetOption(sender->GetTemperatureScale()); // has to happen before AmbientAirTempToTextBox is called
	heatSimulation->SetChecked(sender->GetHeatSimulation());
	ambientHeatSimulation->SetChecked(sender->GetAmbientHeatSimulation());
	newtonianGravity->SetChecked(sender->GetNewtonianGravity());
	waterEqualisation->SetChecked(sender->GetWaterEqualisation());
	airMode->SetOption(sender->GetAirMode());
	// Initialize air temp and preview only when the options menu is opened, and not when user is actively editing the textbox
	if (!ambientAirTemp->IsFocused())
	{
		float airTemp = sender->GetAmbientAirTemperature();
		UpdateAmbientAirTempPreview(airTemp, true);
		AmbientAirTempToTextBox(airTemp);
	}
	gravityMode->SetOption(sender->GetGravityMode());
	customGravityX = sender->GetCustomGravityX();
	customGravityY = sender->GetCustomGravityY();
	decoSpace->SetOption(sender->GetDecoSpace());
	edgeMode->SetOption(sender->GetEdgeMode());
	scale->SetOption(sender->GetScale());
	resizable->SetChecked(sender->GetResizable());
	fullscreen->SetChecked(sender->GetFullscreen());
	altFullscreen->SetChecked(sender->GetAltFullscreen());
	forceIntegerScaling->SetChecked(sender->GetForceIntegerScaling());
	fastquit->SetChecked(sender->GetFastQuit());
	showAvatars->SetChecked(sender->GetShowAvatars());
	mouseClickRequired->SetChecked(sender->GetMouseClickRequired());
	includePressure->SetChecked(sender->GetIncludePressure());
	perfectCirclePressure->SetChecked(sender->GetPerfectCircle());
	momentumScroll->SetChecked(sender->GetMomentumScroll());
}

void OptionsView::AttachController(OptionsController * c_)
{
	c = c_;
}

void OptionsView::OnDraw()
{
	Graphics * g = GetGraphics();
	g->DrawFilledRect(RectSized(Position - Vec2{ 1, 1 }, Size + Vec2{ 2, 2 }), 0x000000_rgb);
	g->DrawRect(RectSized(Position, Size), 0xFFFFFF_rgb);
}

void OptionsView::OnTryExit(ExitMethod method)
{
	c->Exit();
}


OptionsView::~OptionsView() {
}
