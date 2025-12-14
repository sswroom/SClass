#include "Stdafx.h"
#include "Java/JavaFrame.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaFrame::setExtendedState = 0;
jmethodID Java::JavaFrame::setResizable = 0;
jmethodID Java::JavaFrame::setTitle = 0;

Java::JavaFrame::JavaFrame(jobject me) : JavaWindow(me)
{
}

Java::JavaFrame::~JavaFrame()
{
}

void Java::JavaFrame::SetExtendedState(ExtendedState state)
{
	if (setExtendedState == 0)
		setExtendedState = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setExtendedState", "(I)V");
	jniEnv->CallVoidMethod(this->me, setExtendedState, (Int32)state);
}

void Java::JavaFrame::SetResizable(Bool resizable)
{
	if (setResizable == 0)
		setResizable = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setResizable", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setResizable, resizable);
}

void Java::JavaFrame::SetTitle(Text::CStringNN title)
{
	if (setTitle == 0)
		setTitle = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setTitle", "(Ljava/lang/String;)V");
	jstring s;
	jniEnv->CallVoidMethod(this->me, setTitle, s = jniEnv->NewStringUTF((const Char*)title.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
}

jclass Java::JavaFrame::GetClass()
{
	return jniEnv->FindClass("java/awt/Frame");
}
