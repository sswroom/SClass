#include "Stdafx.h"
#include "UI/GUICore.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"

UI::GUICore::~GUICore()
{
}

NN<UI::GUIListView> UI::GUICore::NewListView(NN<UI::GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount)
{
	NN<UI::GUIListView> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIListView(*this, parent, lvstyle, colCount));
	return ctrl;
}

NN<UI::GUIPanel> UI::GUICore::NewPanel(NN<UI::GUIClientControl> parent)
{
	NN<UI::GUIPanel> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIPanel(*this, parent));
	return ctrl;
}

NN<UI::GUIPictureBoxDD> UI::GUICore::NewPictureBoxDD(NN<UI::GUIClientControl> parent, NN<Media::ColorManagerSess> colorSess, Bool allowEnlarge, Bool directMode)
{
	NN<UI::GUIPictureBoxDD> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIPictureBoxDD(*this, parent, colorSess, allowEnlarge, directMode));
	return ctrl;
}
