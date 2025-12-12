#include "Stdafx.h"
#include "Java/JavaContainer.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaContainer::JavaContainer(jobject me) : JavaComponent(me)
{
}

Java::JavaContainer::~JavaContainer()
{
}

jclass Java::JavaContainer::GetClass()
{
	return jniEnv->FindClass("java/awt/Container");
}
