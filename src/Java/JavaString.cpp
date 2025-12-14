#include "Stdafx.h"
#include "Java/JavaString.h"

extern "C"
{
	extern JNIEnv *jniEnv;
}

Java::JavaString::JavaString(jobject me) : JavaObject(me)
{
}

Java::JavaString::~JavaString()
{
}

UnsafeArray<UTF8Char> Java::JavaString::Get(UnsafeArray<UTF8Char> sbuff)
{
	jboolean isCopy;
	const char *s = jniEnv->GetStringUTFChars((jstring)this->me, &isCopy);
	sbuff = Text::StrConcat(sbuff, (const UTF8Char*)s);
	jniEnv->ReleaseStringUTFChars((jstring)this->me, s);
	return sbuff;
}

void Java::JavaString::Get(NN<Text::StringBuilderUTF8> sb)
{
	jboolean isCopy;
	const char *s = jniEnv->GetStringUTFChars((jstring)this->me, &isCopy);
	sb->AppendSlow((const UTF8Char*)s);
	jniEnv->ReleaseStringUTFChars((jstring)this->me, s);
}

jclass Java::JavaString::GetClass()
{
	return jniEnv->FindClass("java/lang/String");
}
