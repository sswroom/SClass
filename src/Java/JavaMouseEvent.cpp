#include "Stdafx.h"
#include "Java/JavaMouseEvent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaMouseEvent::getButton = 0;
jmethodID Java::JavaMouseEvent::getX = 0;
jmethodID Java::JavaMouseEvent::getXOnScreen = 0;
jmethodID Java::JavaMouseEvent::getY = 0;
jmethodID Java::JavaMouseEvent::getYOnScreen = 0;

Java::JavaMouseEvent::JavaMouseEvent(jobject me) : JavaInputEvent(me)
{
}

Java::JavaMouseEvent::~JavaMouseEvent()
{
}

Java::JavaMouseEvent::Button Java::JavaMouseEvent::GetButton()
{
	if (getButton == 0)
		getButton = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getButton", "()I");
	return (Button)jniEnv->CallIntMethod(this->me, getButton);
}

Int32 Java::JavaMouseEvent::GetX()
{
	if (getX == 0)
		getX = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getX", "()I");
	return jniEnv->CallIntMethod(this->me, getX);
}

Int32 Java::JavaMouseEvent::GetXOnScreen()
{
	if (getXOnScreen == 0)
		getXOnScreen = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getXOnScreen", "()I");
	return jniEnv->CallIntMethod(this->me, getXOnScreen);
}

Int32 Java::JavaMouseEvent::GetY()
{
	if (getY == 0)
		getY = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getY", "()I");
	return jniEnv->CallIntMethod(this->me, getY);
}

Int32 Java::JavaMouseEvent::GetYOnScreen()
{
	if (getYOnScreen == 0)
		getYOnScreen = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getYOnScreen", "()I");
	return jniEnv->CallIntMethod(this->me, getYOnScreen);
}

jclass Java::JavaMouseEvent::GetClass()
{
	return jniEnv->FindClass("java/awt/event/MouseEvent");
}

UI::GUIControl::MouseButton Java::JavaMouseEvent::Button2MouseButton(Button btn)
{
	if ((IntOS)btn == 0)
	{
		return UI::GUIControl::MBTN_LEFT;
	}
	else
	{
		return (UI::GUIControl::MouseButton)((IntOS)btn - 1);
	}
}
