#include "Stdafx.h"
#include "Java/JavaComponent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jclass Java::JavaComponent::GetClass()
{
	return jniEnv->FindClass("java/awt/Component");
}

Single Java::JavaComponent::HorizontalAlignmentGetInt(HorizontalAlignment halign)
{
	return 0;
}

Single Java::JavaComponent::VerticalAlignmentGetInt(VerticalAlignment valign)
{
	return 0;
}
