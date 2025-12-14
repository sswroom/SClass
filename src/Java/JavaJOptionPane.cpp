#include "Stdafx.h"
#include "Java/JavaJOptionPane.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

void Java::JavaJOptionPane::ShowMessageDialog(Optional<JavaComponent> parentComponent, Text::CStringNN message, Text::CStringNN title, MessageType messageType)
{
	jclass cls = GetClass();
	jmethodID meth = jniEnv->GetStaticMethodID(cls, "showMessageDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;I)V");
	NN<JavaComponent> nnparentComponent;
	jobject component = 0;
	if (parentComponent.SetTo(nnparentComponent))
	{
		component = nnparentComponent->GetJObject();
	}
	jstring s;
	jstring s2;
	jniEnv->CallStaticVoidMethod(cls, meth, component, s = jniEnv->NewStringUTF((const Char*)message.v.Ptr()), s2 = jniEnv->NewStringUTF((const Char*)title.v.Ptr()), MessageTypeGetInt(messageType));
	jniEnv->DeleteLocalRef(s);
	jniEnv->DeleteLocalRef(s2);
}

Java::JavaJOptionPane::SelectedOption Java::JavaJOptionPane::ShowConfirmDialog(Optional<JavaComponent> parentComponent, Text::CStringNN message, Text::CStringNN title, OptionType optionType, MessageType messageType)
{
	jclass cls = GetClass();
	jmethodID meth = jniEnv->GetStaticMethodID(cls, "showConfirmDialog", "(Ljava/awt/Component;Ljava/lang/Object;Ljava/lang/String;II)V");
	NN<JavaComponent> nnparentComponent;
	jobject component = 0;
	if (parentComponent.SetTo(nnparentComponent))
	{
		component = nnparentComponent->GetJObject();
	}
	jstring s;
	jstring s2;
	SelectedOption opt = (SelectedOption)jniEnv->CallStaticIntMethod(cls, meth, component, s = jniEnv->NewStringUTF((const Char*)message.v.Ptr()), s2 = jniEnv->NewStringUTF((const Char*)title.v.Ptr()), OptionTypeGetInt(optionType), MessageTypeGetInt(messageType));
	jniEnv->DeleteLocalRef(s);
	jniEnv->DeleteLocalRef(s2);
	return opt;
}

jclass Java::JavaJOptionPane::GetClass()
{
	return jniEnv->FindClass("javax/swing/JOptionPane");
}

Int32 Java::JavaJOptionPane::OptionTypeGetInt(OptionType optionType)
{
	return (Int32)optionType;
}

Int32 Java::JavaJOptionPane::MessageTypeGetInt(MessageType messageType)
{
	return (Int32)messageType;
}
