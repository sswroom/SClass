#include "Stdafx.h"
#include "Java/JavaLayoutManager.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaLayoutManager::JavaLayoutManager(jobject me) : JavaObject(me)
{
}

Java::JavaLayoutManager::~JavaLayoutManager()
{
}

jclass Java::JavaLayoutManager::GetClass()
{
	return jniEnv->FindClass("java/awt/LayoutManager");
}
