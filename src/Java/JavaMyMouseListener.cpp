#include "Stdafx.h"
#include "Java/JavaMouseEvent.h"
#include "Java/JavaMyMouseListener.h"

extern "C"
{
	extern JNIEnv *jniEnv;

JNIEXPORT void JNICALL Java_MyMouseListener_mouseClicked(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMouseClickedHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::GUIControl::MouseEventHandler hdlr = (UI::GUIControl::MouseEventHandler)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr)
	{
		Java::JavaMouseEvent evt(e);
		hdlr(userObj, Math::Coord2D<OSInt>(evt.GetXOnScreen(), evt.GetYOnScreen()), Java::JavaMouseEvent::Button2MouseButton(evt.GetButton()));
	}
}

JNIEXPORT void JNICALL Java_MyMouseListener_mouseEntered(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMouseEnteredHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::GUIControl::MouseEventHandler hdlr = (UI::GUIControl::MouseEventHandler)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr)
	{
		Java::JavaMouseEvent evt(e);
		hdlr(userObj, Math::Coord2D<OSInt>(evt.GetXOnScreen(), evt.GetYOnScreen()), Java::JavaMouseEvent::Button2MouseButton(evt.GetButton()));
	}
}

JNIEXPORT void JNICALL Java_MyMouseListener_mouseExited(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMouseExitedHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::GUIControl::MouseEventHandler hdlr = (UI::GUIControl::MouseEventHandler)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr)
	{
		Java::JavaMouseEvent evt(e);
		hdlr(userObj, Math::Coord2D<OSInt>(evt.GetXOnScreen(), evt.GetYOnScreen()), Java::JavaMouseEvent::Button2MouseButton(evt.GetButton()));
	}
}

JNIEXPORT void JNICALL Java_MyMouseListener_mousePressed(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMousePressedHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::GUIControl::MouseEventHandler hdlr = (UI::GUIControl::MouseEventHandler)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr)
	{
		Java::JavaMouseEvent evt(e);
		hdlr(userObj, Math::Coord2D<OSInt>(evt.GetXOnScreen(), evt.GetYOnScreen()), Java::JavaMouseEvent::Button2MouseButton(evt.GetButton()));
	}
}

JNIEXPORT void JNICALL Java_MyMouseListener_mouseReleased(JNIEnv *env, jobject obj, jobject e)
{
	jclass cls = env->GetObjectClass(obj);
	jmethodID mid = env->GetMethodID(cls, "getMouseReleasedHdlr", "()J");
	jmethodID mid2 = env->GetMethodID(cls, "getUserObj", "()J");
	UI::GUIControl::MouseEventHandler hdlr = (UI::GUIControl::MouseEventHandler)env->CallLongMethod(obj, mid);
	AnyType userObj = (void*)(OSInt)env->CallLongMethod(obj, mid2);
	if (hdlr)
	{
		Java::JavaMouseEvent evt(e);
		hdlr(userObj, Math::Coord2D<OSInt>(evt.GetXOnScreen(), evt.GetYOnScreen()), Java::JavaMouseEvent::Button2MouseButton(evt.GetButton()));
	}
}
}

Java::JavaMyMouseListener::JavaMyMouseListener(AnyType userObj) : JavaObject(NewObject(userObj))
{
}

Java::JavaMyMouseListener::~JavaMyMouseListener()
{
}

void Java::JavaMyMouseListener::HandleMouseClicked(UI::GUIControl::MouseEventHandler hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setMouseClickedHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

void Java::JavaMyMouseListener::HandleMouseEntered(UI::GUIControl::MouseEventHandler hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setMouseEnteredHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

void Java::JavaMyMouseListener::HandleMouseExited(UI::GUIControl::MouseEventHandler hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setMouseExitedHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

void Java::JavaMyMouseListener::HandleMousePressed(UI::GUIControl::MouseEventHandler hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setMousePressedHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

void Java::JavaMyMouseListener::HandleMouseReleased(UI::GUIControl::MouseEventHandler hdlr)
{
	jclass cls = jniEnv->GetObjectClass(this->me);
	jmethodID mid = jniEnv->GetMethodID(cls, "setMouseReleasedHdlr", "(J)V");
	jniEnv->CallVoidMethod(this->me, mid, (Int64)hdlr);
}

jclass Java::JavaMyMouseListener::GetClass()
{
	return jniEnv->FindClass("MyMouseListener");
}

jobject Java::JavaMyMouseListener::NewObject(AnyType userObj)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(J)V");
	return jniEnv->NewObject(cls, mid, (Int64)userObj.GetOSInt());
}
