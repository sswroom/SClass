#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Java/JavaCore.h"
#include <jni.h>
#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl);

extern "C"
{
	void *jniEnv;
}

struct LinuxProgControl : public Core::ProgControl
{
	UTF8Char **argv;
	OSInt argc;
};

void LinuxProgControl_OnSignal(Int32 sigNum)
{
}

void __stdcall LinuxProgControl_WaitForExit(NN<Core::ProgControl> progCtrl)
{
/*	signal(SIGINT, LinuxProgControl_OnSignal);
	signal(SIGPIPE, LinuxProgControl_OnSignal);
	signal(SIGTERM, LinuxProgControl_OnSignal);
	pause();*/
//	getchar();
}

Optional<UI::GUICore> __stdcall Core::ProgControl::CreateGUICore(NN<Core::ProgControl> progCtrl)
{
	NN<UI::Java::JavaCore> ui;
	NEW_CLASSNN(ui, UI::Java::JavaCore());
	return ui;
}

UTF8Char **__stdcall LinuxProgControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UOSInt> cmdCnt)
{
	NN<LinuxProgControl> ctrl = NN<LinuxProgControl>::ConvertFrom(progCtrl);
	cmdCnt.Set((UOSInt)ctrl->argc);
	return ctrl->argv;
}

void LinuxProgControl_Create(LinuxProgControl *ctrl)
{
	ctrl->WaitForExit = LinuxProgControl_WaitForExit;
	ctrl->GetCommandLines = LinuxProgControl_GetCommandLines;
	ctrl->SignalExit = LinuxProgControl_WaitForExit;
	ctrl->SignalRestart = LinuxProgControl_WaitForExit;
}

void LinuxProgControl_Destroy(LinuxProgControl *ctrl)
{
}

extern "C"
{
	JNIEXPORT jint JNICALL Java_MyMain_myMain(JNIEnv *env, jclass cls, jobjectArray args)
	{
		Int32 ret;
		LinuxProgControl conCtrl;
		OSInt i;
		jobject sobj;
		const jchar *strArr;
		jboolean isCopy;
		Core::CoreStart();
		jniEnv = env;

		conCtrl.argc = env->GetArrayLength(args) + 1;
		conCtrl.argv = MemAlloc(UTF8Char*, conCtrl.argc);
		conCtrl.argv[0] = (UTF8Char*)Text::StrCopyNew("java");
		i = 1;
		while (i < conCtrl.argc)
		{
			sobj = env->GetObjectArrayElement(args, i - 1);
			strArr = env->GetStringChars((jstring)sobj, &isCopy);
			conCtrl.argv[i] = (UTF8Char*)Text::StrToUTF8New((const UTF16Char*)strArr);
			i++;
		}

		LinuxProgControl_Create(&conCtrl);
		ret = MyMain(conCtrl);
		LinuxProgControl_Destroy(&conCtrl);
		i = conCtrl.argc;
		while (i-- > 0)
		{
			Text::StrDelNew(conCtrl.argv[i]);
		}
		MemFree(conCtrl.argv);
		Core::CoreEnd();
		return ret;
	}
}
