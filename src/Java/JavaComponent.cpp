#include "Stdafx.h"
#include "Java/JavaComponent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaComponent::getHeight = 0;
jmethodID Java::JavaComponent::getWidth = 0;
jmethodID Java::JavaComponent::requestFocus = 0;

Java::JavaComponent::JavaComponent(jobject me) : JavaObject(me)
{
}

Java::JavaComponent::~JavaComponent()
{
}

Int32 Java::JavaComponent::GetHeight()
{
	if (getHeight == 0)
		getHeight = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getHeight", "()I");
	return jniEnv->CallIntMethod(this->me, getHeight);
}

Int32 Java::JavaComponent::GetWidth()
{
	if (getWidth == 0)
		getWidth = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getWidth", "()I");
	return jniEnv->CallIntMethod(this->me, getWidth);
}

void Java::JavaComponent::RequestFocus()
{
	if (requestFocus == 0)
		requestFocus = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "requestFocus", "()V");
	jniEnv->CallVoidMethod(this->me, requestFocus);
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
