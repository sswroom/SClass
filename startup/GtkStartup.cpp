#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Media/GTKDrawEngine.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKCore.h"
#include <gtk/gtk.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> ctrl);

struct GtkProgControl : public Core::IProgControl
{
	UTF8Char **argv;
	UOSInt argc;
};

void __stdcall GtkProgControl_WaitForExit(NotNullPtr<Core::IProgControl> progCtrl)
{
}

UTF8Char ** __stdcall GtkProgControl_GetCommandLines(NotNullPtr<Core::IProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	GtkProgControl *ctrl = (GtkProgControl*)progCtrl.Ptr();
	cmdCnt.Set(ctrl->argc);
	return ctrl->argv;
}

void GtkProgControl_Create(NotNullPtr<GtkProgControl> ctrl, UOSInt argc, Char **argv)
{
	ctrl->argv = (UTF8Char**)argv;
	ctrl->argc = argc;
	ctrl->WaitForExit = GtkProgControl_WaitForExit;
	ctrl->GetCommandLines = GtkProgControl_GetCommandLines;
	ctrl->SignalExit = GtkProgControl_WaitForExit;
	ctrl->SignalRestart = GtkProgControl_WaitForExit;
}

void GtkProgControl_Destroy(NotNullPtr<GtkProgControl> ctrl)
{
}

Optional<UI::GUICore> Core::IProgControl::CreateGUICore(NotNullPtr<Core::IProgControl> progCtrl)
{
	NotNullPtr<UI::GTK::GTKCore> ui;
	NEW_CLASSNN(ui, UI::GTK::GTKCore());
	return ui;
}

int main(int argc, char *argv[])
{
	Int32 ret;
	GtkProgControl ctrl;
	//signal(SIGCHLD, SIG_IGN);

	gtk_init(&argc, &argv);
	Core::CoreStart();
	GtkProgControl_Create(ctrl, (UOSInt)argc, argv);
	ret = MyMain(ctrl);
	GtkProgControl_Destroy(ctrl);
	Core::CoreEnd();
	return ret;
}
