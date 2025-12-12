#include "Stdafx.h"
#include "UI/JavaUI/JUIFileDialog.h"

UI::JavaUI::JUIFileDialog::JUIFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::JavaUI::JUIFileDialog::~JUIFileDialog()
{
}

Bool UI::JavaUI::JUIFileDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	return false;
}
