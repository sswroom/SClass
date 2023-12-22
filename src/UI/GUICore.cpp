#include "Stdafx.h"
#include "UI/GUICore.h"
#include "UI/GUIListView.h"

UI::GUICore::~GUICore()
{
}

NotNullPtr<UI::GUIListView> UI::GUICore::NewListView(NotNullPtr<UI::GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount)
{
	NotNullPtr<UI::GUIListView> ctrl;
	NEW_CLASSNN(ctrl, UI::GUIListView(*this, parent, lvstyle, colCount));
	return ctrl;
}
