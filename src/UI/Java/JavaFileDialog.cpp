#include "Stdafx.h"
#include "UI/Java/JavaFileDialog.h"

UI::Java::JavaFileDialog::JavaFileDialog(UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName, UnsafeArray<const WChar> dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::Java::JavaFileDialog::~JavaFileDialog()
{
}

Bool UI::Java::JavaFileDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	return false;
}
