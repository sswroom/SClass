#include "Stdafx.h"
#include "UI/GUICore.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

UI::GUICore::~GUICore()
{
}

NotNullPtr<UI::GUIListView> UI::GUICore::NewListView(NotNullPtr<UI::GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount)
{
	NotNullPtr<UI::GUIListView> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIListView(*this, parent, lvstyle, colCount));
	return ctrl;
}

NotNullPtr<UI::GUIPanel> UI::GUICore::NewPanel(NotNullPtr<UI::GUIClientControl> parent)
{
	NotNullPtr<UI::GUIPanel> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIPanel(*this, parent));
	return ctrl;
}
