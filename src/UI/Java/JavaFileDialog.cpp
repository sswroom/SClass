#include "Stdafx.h"
#include "UI/Java/JavaFileDialog.h"

UI::Java::JavaFileDialog::JavaFileDialog(const WChar *compName, const WChar *appName, const WChar *dialogName, Bool isSave) : UI::GUIFileDialog(compName, appName, dialogName, isSave)
{
}

UI::Java::JavaFileDialog::~JavaFileDialog()
{
}

Bool UI::Java::JavaFileDialog::ShowDialog(ControlHandle *ownerHandle)
{
	return false;
}
