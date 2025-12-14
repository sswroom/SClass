#include "Stdafx.h"
#include "Java/JavaJTextComponent.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

jmethodID Java::JavaJTextComponent::getText = 0;
jmethodID Java::JavaJTextComponent::selectAll = 0;
jmethodID Java::JavaJTextComponent::setEditable = 0;
jmethodID Java::JavaJTextComponent::setText = 0;

Java::JavaJTextComponent::JavaJTextComponent(jobject me) : JavaJComponent(me)
{
}

Java::JavaJTextComponent::~JavaJTextComponent()
{
}

Java::JavaString Java::JavaJTextComponent::GetText()
{
	if (getText == 0)
		getText = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "getText", "()Ljava/lang/String;");
	return JavaString(jniEnv->CallObjectMethod(this->me, getText));
}

void Java::JavaJTextComponent::SelectAll()
{
	if (selectAll == 0)
		selectAll = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "selectAll", "()V");
	jniEnv->CallVoidMethod(this->me, selectAll);
}

void Java::JavaJTextComponent::SetEditable(Bool b)
{
	if (setEditable == 0)
		setEditable = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setEditable", "(Z)V");
	jniEnv->CallVoidMethod(this->me, setEditable, b);
}

void Java::JavaJTextComponent::SetText(Text::CStringNN t)
{
	if (setText == 0)
		setText = jniEnv->GetMethodID(jniEnv->GetObjectClass(this->me), "setText", "(Ljava/lang/String;)V");
	jstring s;
	jniEnv->CallVoidMethod(this->me, setText, s = jniEnv->NewStringUTF((const Char*)t.v.Ptr()));
	jniEnv->DeleteLocalRef(s);
}

jclass Java::JavaJTextComponent::GetClass()
{
	return jniEnv->FindClass("javax/swing/JTextComponent");
}
