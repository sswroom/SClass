#include "Stdafx.h"
#include "Java/JavaComponent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaComponent::getHeight = 0;
jmethodID Java::JavaComponent::getWidth = 0;
jmethodID Java::JavaComponent::isVisible = 0;
jmethodID Java::JavaComponent::requestFocus = 0;
jmethodID Java::JavaComponent::setLocation = 0;
jmethodID Java::JavaComponent::setSize = 0;
jmethodID Java::JavaComponent::setVisible = 0;

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

Bool Java::JavaComponent::IsVisible()
{
	if (isVisible == 0)
		isVisible = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "isVisible", "()Z");
	return jniEnv->CallBooleanMethod(this->me, isVisible);
}

void Java::JavaComponent::RequestFocus()
{
	if (requestFocus == 0)
		requestFocus = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "requestFocus", "()V");
	jniEnv->CallVoidMethod(this->me, requestFocus);
}

void Java::JavaComponent::SetLocation(Int32 x, Int32 y)
{
	if (setLocation == 0)
		setLocation = jniEnv->GetMethodID(GetClass(), "setLocation", "(II)V");
	jniEnv->CallVoidMethod(this->me, setLocation, x, y);
}

void Java::JavaComponent::SetSize(Int32 width, Int32 height)
{
	if (setSize == 0)
		setSize = jniEnv->GetMethodID(GetClass(), "setSize", "(II)V");
	jniEnv->CallVoidMethod(this->me, setSize, width, height);
}

void Java::JavaComponent::SetVisible(Bool b)
{
	if (setVisible == 0)
		setVisible = jniEnv->GetMethodID(GetClass(), "setVisible", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setVisible, b);
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
