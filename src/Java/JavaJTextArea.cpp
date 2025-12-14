#include "Stdafx.h"
#include "Java/JavaJTextArea.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJTextArea::setLineWrap = 0;

Java::JavaJTextArea::JavaJTextArea(Text::CStringNN text) : JavaJTextComponent(NewObject(text))
{
}

Java::JavaJTextArea::~JavaJTextArea()
{
}

void Java::JavaJTextArea::SetLineWrap(Bool wrap)
{
	if (setLineWrap == 0)
		setLineWrap = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setLineWrap", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setLineWrap, wrap);
}

jclass Java::JavaJTextArea::GetClass()
{
	return jniEnv->FindClass("javax/swing/JTextArea");
}

jobject Java::JavaJTextArea::NewObject(Text::CStringNN text)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
	jstring s;
	jobject o = jniEnv->NewObject(cls, mid, s = jniEnv->NewStringUTF((const Char*)text.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
	return o;
}
