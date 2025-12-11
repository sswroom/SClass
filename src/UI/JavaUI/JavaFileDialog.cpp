#include "Stdafx.h"
#include "UI/JavaUI/JavaFileDialog.h"

UI::JavaUI::JavaFileDialog::JavaFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::JavaUI::JavaFileDialog::~JavaFileDialog()
{
}

Bool UI::JavaUI::JavaFileDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	return false;
}
