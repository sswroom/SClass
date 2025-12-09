#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Java/JavaMessageDialog.h"
#include <stdio.h>

void UI::Java::JavaMessageDialog::ShowOK(JNIEnv *env, Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	jclass cls = env->FindClass("javax/swing/JOptionPane");
	if (cls == 0)
		return;
	jmethodID meth = env->GetStaticMethodID(cls, "showMessageDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;I)V");
	jfieldID fid = env->GetStaticFieldID(cls, "PLAIN_MESSAGE", "I");
	if (meth == 0 || fid == 0)
		return;
	env->CallStaticVoidMethod(cls, meth, 0, env->NewStringUTF((const Char*)message.v.Ptr()), env->NewStringUTF((const Char*)title.v.Ptr()), env->GetStaticIntField(cls, fid));
}

Bool UI::Java::JavaMessageDialog::ShowYesNo(JNIEnv *env, Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	jclass cls = env->FindClass("javax.swing.JOptionPane");
	if (cls == 0)
		return false;
	jmethodID meth = env->GetStaticMethodID(cls, "showConfirmDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;II)V");
	jfieldID fid = env->GetStaticFieldID(cls, "PLAIN_MESSAGE", "I");
	jfieldID fid2 = env->GetStaticFieldID(cls, "YES_NO_OPTION", "I");
	jfieldID fid3 = env->GetStaticFieldID(cls, "YES_OPTION", "I");
	if (meth == 0 || fid == 0 || fid2 == 0 || fid3 == 0)
		return false;
	return env->CallStaticIntMethod(cls, meth, 0, env->NewStringUTF((const Char*)message.v.Ptr()), env->NewStringUTF((const Char*)title.v.Ptr()), env->GetStaticIntField(cls, fid2), env->GetStaticIntField(cls, fid)) == env->GetStaticIntField(cls, fid3);
}
