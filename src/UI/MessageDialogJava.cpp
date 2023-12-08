#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"
#include <jni.h>
#include <stdio.h>

extern "C"
{
	extern void *jniEnv;
}

void UI::MessageDialog::ShowDialog(Text::CStringNN message, Text::CStringNN title, UI::GUIControl *ctrl)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->FindClass("javax/swing/JOptionPane");
	if (cls == 0)
		return;
	jmethodID meth = env->GetStaticMethodID(cls, "showMessageDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;I)V");
	jfieldID fid = env->GetStaticFieldID(cls, "PLAIN_MESSAGE", "I");
	if (meth == 0 || fid == 0)
		return;
	env->CallStaticVoidMethod(cls, meth, 0, env->NewStringUTF((const Char*)message.v), env->NewStringUTF((const Char*)title.v), env->GetStaticIntField(cls, fid));
}

Bool UI::MessageDialog::ShowYesNoDialog(Text::CStringNN message, Text::CStringNN title, UI::GUIControl *ctrl)
{
	JNIEnv *env = (JNIEnv*)jniEnv;
	jclass cls = env->FindClass("javax.swing.JOptionPane");
	if (cls == 0)
		return false;
	jmethodID meth = env->GetStaticMethodID(cls, "showConfirmDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;II)V");
	jfieldID fid = env->GetStaticFieldID(cls, "PLAIN_MESSAGE", "I");
	jfieldID fid2 = env->GetStaticFieldID(cls, "YES_NO_OPTION", "I");
	jfieldID fid3 = env->GetStaticFieldID(cls, "YES_OPTION", "I");
	if (meth == 0 || fid == 0 || fid2 == 0 || fid3 == 0)
		return false;
	return env->CallStaticIntMethod(cls, meth, 0, env->NewStringUTF((const Char*)message.v), env->NewStringUTF((const Char*)title.v), env->GetStaticIntField(cls, fid2), env->GetStaticIntField(cls, fid)) == env->GetStaticIntField(cls, fid3);
}
