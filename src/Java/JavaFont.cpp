#include "Stdafx.h"
#include "Java/JavaFont.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaFont::JavaFont(Text::CString name, Style style, Int32 size) : JavaObject(NewObject(name, style, size))
{
}

Java::JavaFont::~JavaFont()
{
}

jclass Java::JavaFont::GetClass()
{
	return jniEnv->FindClass("java.awt.Font");
}

jobject Java::JavaFont::NewObject(Text::CString name, Style style, Int32 size)
{
	jclass cls = GetClass();
	jmethodID mid = jniEnv->GetMethodID(cls, "<init>", "(Ljava/lang/String;II)V");
	jstring s = 0;
	Text::CStringNN nnname;
	if (name.SetTo(nnname))
	{
		s = jniEnv->NewStringUTF((const Char*)nnname.v.Ptr());
	}
	jobject o = jniEnv->NewObject(cls, mid, s, (Int32)style, size);
	if (s) jniEnv->DeleteLocalRef(s);
	return o;
}
