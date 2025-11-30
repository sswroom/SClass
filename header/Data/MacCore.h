#ifndef _SM_DATA_MACCORE
#define _SM_DATA_MACCORE
#include "Stdafx.h"
#include "Text/StringBuilderUTF8.h"
#undef UTF8Char
#undef UTF16Char
#undef UTF32Char
#include <stdlib.h>
#include <CoreFoundation/CoreFoundation.h>
#define UTF8Char UTF8Ch
#define UTF16Char UTF16Ch
#define UTF32Char UTF32Ch

namespace Data
{
	class MacString;
	class MacType
	{
	protected:
		CFTypeRef val;

	public:
		MacType(CFTypeRef val);
		virtual ~MacType();

		Bool IsNull() const;
		Bool IsString() const;
		Bool IsNumber() const;
		Bool IsBoolean() const;
		Bool IsData() const;
		Bool IsArray() const;
		Bool IsDictionary() const;
		void Release();
		Optional<MacString> CopyTypeDesc() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacString : public MacType
	{
	public:
		MacString(CFStringRef val);
		virtual ~MacString();

		const UTF8Char *Ptr() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacNumber : public MacType
	{
	public:
		MacNumber(CFNumberRef val);
		virtual ~MacNumber();

		Bool IsFloat() const;
		Double GetDouble() const;
		Int32 GetInt32() const;
		Int64 GetInt64() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacBoolean : public MacType
	{
	public:
		MacBoolean(CFBooleanRef val);
		virtual ~MacBoolean();

		Bool GetValue() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacData : public MacType
	{
	public:
		MacData(CFDataRef val);
		virtual ~MacData();

		const UInt8 *Ptr() const;
		UOSInt GetSize() const;
		Data::ByteArrayR GetArray() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacArray : public MacType
	{
	public:
		MacArray(CFArrayRef val);
		virtual ~MacArray();

		CFTypeRef GetItem(UOSInt i) const;
		UOSInt GetCount() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class MacDictionary : public MacType
	{
	public:
		MacDictionary(CFDictionaryRef val);
		virtual ~MacDictionary();

		CFTypeRef Get(Text::CString name) const;
		CFTypeRef Get(CFStringRef name) const;
		UOSInt GetCount() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
