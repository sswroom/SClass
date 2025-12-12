#include "Stdafx.h"
#include "Java/JavaJFrame.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJFrame::getContentPane = 0;

Java::JavaJFrame::JavaJFrame() : JavaFrame(NewObject())
{
}

Java::JavaJFrame::JavaJFrame(jobject me) : JavaFrame(me)
{
}

Java::JavaJFrame::~JavaJFrame()
{
}

Java::JavaContainer Java::JavaJFrame::GetContentPane()
{
	if (getContentPane == 0)
		getContentPane = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getContentPane", "()Ljava/awt/Container;");
	return JavaContainer(jniEnv->CallObjectMethod(this->me, getContentPane));
}

jclass Java::JavaJFrame::GetClass()
{
	return jniEnv->FindClass("javax/swing/JFrame");
}

jobject Java::JavaJFrame::NewObject()
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "()V");
	return jniEnv->NewObject(cls, mid);
}
