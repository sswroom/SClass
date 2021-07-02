#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Media/GTKDrawEngine.h"
#include "Text/MyString.h"
#include "UI/GUICoreGTK.h"
#include <gtk/gtk.h>

Int32 MyMain(Core::IProgControl *ctrl);

struct GtkProgControl : public Core::IProgControl
{
	UTF8Char **argv;
	OSInt argc;
};

void __stdcall GtkProgControl_WaitForExit(Core::IProgControl *progCtrl)
{
}

UTF8Char ** __stdcall GtkProgControl_GetCommandLines(Core::IProgControl *progCtrl, OSInt *cmdCnt)
{
	GtkProgControl *ctrl = (GtkProgControl*)progCtrl;
	*cmdCnt = ctrl->argc;
	return ctrl->argv;
}

void GtkProgControl_Create(GtkProgControl *ctrl, OSInt argc, Char **argv)
{
	ctrl->argv = (UTF8Char**)argv;
	ctrl->argc = argc;
	ctrl->WaitForExit = GtkProgControl_WaitForExit;
	ctrl->GetCommandLines = GtkProgControl_GetCommandLines;
}

void GtkProgControl_Destroy(GtkProgControl *ctrl)
{
}

UI::GUICore *Core::IProgControl::CreateGUICore(Core::IProgControl *progCtrl)
{
	UI::GUICoreGTK *ui;
	NEW_CLASS(ui, UI::GUICoreGTK());
	return ui;
}

int main(int argc, char *argv[])
{
	Int32 ret;
	GtkProgControl ctrl;
	//signal(SIGCHLD, SIG_IGN);

	gtk_init(&argc, &argv);
	Core::CoreStart();
	GtkProgControl_Create(&ctrl, argc, argv);
	ret = MyMain(&ctrl);
	GtkProgControl_Destroy(&ctrl);
	Core::CoreEnd();
	return ret;
}
