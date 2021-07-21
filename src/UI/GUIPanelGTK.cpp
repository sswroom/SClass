#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include <gtk/gtk.h>


void UI::GUIPanel::Init(void *hInst)
{
}

void UI::GUIPanel::Deinit(void *hInst)
{
}

void UI::GUIPanel::UpdateScrollBars()
{
}

UI::GUIPanel::GUIPanel(UI::GUICore *ui, void *parentHWnd) : UI::GUIClientControl(ui, 0)
{
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;
	this->hwnd = gtk_fixed_new();
}

UI::GUIPanel::GUIPanel(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIClientControl(ui, parent)
{
	this->minW = 0;
	this->minH = 0;
	this->scrollH = false;
	this->scrollV = false;
	this->currScrX = 0;
	this->currScrY = 0;

	this->hwnd = gtk_fixed_new();
	parent->AddChild(this);
	this->Show();
}

UI::GUIPanel::~GUIPanel()
{
}

Bool UI::GUIPanel::IsChildVisible()
{
	return true;
}

void UI::GUIPanel::GetClientOfst(Double *x, Double *y)
{
	if (x)
		*x = -this->currScrX;
	if (y)
		*y = -this->currScrY;
}

void UI::GUIPanel::GetClientSize(Double *w, Double *h)
{
	Double cliW;
	Double cliH;
	this->GetSize(&cliW, &cliH);
	if (w)
		*w = cliW;
	if (h)
		*h = cliH;
}

const UTF8Char *UI::GUIPanel::GetObjectClass()
{
	return (const UTF8Char*)"Panel";
}

OSInt UI::GUIPanel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPanel::SetMinSize(Int32 minW, Int32 minH)
{
	this->minW = minW;
	this->minH = minH;
	this->UpdateScrollBars();
}
