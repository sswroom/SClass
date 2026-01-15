#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/JavaUI/JUICore.h"
#include <jni.h>
#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl);

extern "C"
{
	JNIEnv *jniEnv;
}

struct LinuxProgControl : public Core::ProgControl
{
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	IntOS argc;
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
	NN<UI::JavaUI::JUICore> ui;
	NEW_CLASSNN(ui, UI::JavaUI::JUICore());
	return ui;
}

UnsafeArray<UnsafeArray<UTF8Char>> __stdcall LinuxProgControl_GetCommandLines(NN<Core::ProgControl> progCtrl, OutParam<UIntOS> cmdCnt)
{
	NN<LinuxProgControl> ctrl = NN<LinuxProgControl>::ConvertFrom(progCtrl);
	cmdCnt.Set((UIntOS)ctrl->argc);
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
		IntOS i;
		jobject sobj;
		const jchar *strArr;
		jboolean isCopy;
		Core::CoreStart();
		jniEnv = env;

		conCtrl.argc = env->GetArrayLength(args) + 1;
		conCtrl.argv = MemAllocArr(UnsafeArray<UTF8Char>, conCtrl.argc);
		conCtrl.argv[0] = UnsafeArray<UTF8Char>::ConvertFrom(Text::StrCopyNew(U8STR("java")));
		i = 1;
		while (i < conCtrl.argc)
		{
			sobj = env->GetObjectArrayElement(args, i - 1);
			strArr = env->GetStringChars((jstring)sobj, &isCopy);
			conCtrl.argv[i] = UnsafeArray<UTF8Char>::ConvertFrom(Text::StrToUTF8New((const UTF16Char*)strArr));
			i++;
		}

		LinuxProgControl_Create(&conCtrl);
		ret = MyMain(conCtrl);
		LinuxProgControl_Destroy(&conCtrl);
		i = conCtrl.argc;
		while (i-- > 0)
		{
			Text::StrDelNew(UnsafeArray<const UTF8Char>(conCtrl.argv[i]));
		}
		MemFreeArr(conCtrl.argv);
		Core::CoreEnd();
		return ret;
	}
}
