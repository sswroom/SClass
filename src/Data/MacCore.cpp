#include "Stdafx.h"
#include "Data/MacCore.h"
#include "Text/JSText.h"

Data::MacType::MacType(CFTypeRef val)
{
	this->val = val;
}

Data::MacType::~MacType()
{
}

Bool Data::MacType::IsNull() const
{
	return this->val == 0;
}

Bool Data::MacType::IsString() const
{
	return CFGetTypeID(this->val) == CFStringGetTypeID();
}

Bool Data::MacType::IsNumber() const
{
	return CFGetTypeID(this->val) == CFNumberGetTypeID();
}

Bool Data::MacType::IsBoolean() const
{
	return CFGetTypeID(this->val) == CFBooleanGetTypeID();
}

Bool Data::MacType::IsData() const
{
	return CFGetTypeID(this->val) == CFDataGetTypeID();
}

Bool Data::MacType::IsArray() const
{
	return CFGetTypeID(this->val) == CFArrayGetTypeID();
}

Bool Data::MacType::IsDictionary() const
{
	return CFGetTypeID(this->val) == CFDictionaryGetTypeID();
}

void Data::MacType::Release()
{
	CFRelease(this->val);
	DEL_CLASS(this);
}

Optional<Data::MacString> Data::MacType::CopyTypeDesc() const
{
	CFStringRef s = CFCopyTypeIDDescription(CFGetTypeID(this->val));
	if (s)
	{
		return NEW_CLASS_D(MacString(s));
	}
	return 0;
}

void Data::MacType::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (IsNull())
	{
		sb->AppendC(UTF8STRC("null"));
	}
	else if (IsString())
	{
		MacString((CFStringRef)this->val).ToString(sb);
	}
	else if (IsNumber())
	{
		MacNumber((CFNumberRef)this->val).ToString(sb);
	}
	else if (IsBoolean())
	{
		MacBoolean((CFBooleanRef)this->val).ToString(sb);
	}
	else if (IsData())
	{
		MacData((CFDataRef)this->val).ToString(sb);
	}
	else if (IsArray())
	{
		MacArray((CFArrayRef)this->val).ToString(sb);
	}
	else if (IsDictionary())
	{
		MacDictionary((CFDictionaryRef)this->val).ToString(sb);
	}
	else
	{
		MacString *s = this->CopyTypeDesc();
		if (s)
		{
			sb->AppendC(UTF8STRC("Unknown type ("));
			s->ToString(sb);
			sb->AppendUTF8Char(')');
			s->Release();
		}
		else
		{
			sb->AppendC(UTF8STRC("Unknown type"));
		}
	}
}

Data::MacString::MacString(CFStringRef val) : MacType(val)
{
}

Data::MacString::~MacString()
{
}

const UTF8Char *Data::MacString::Ptr() const
{
	CFStringEncoding enc = CFStringGetFastestEncoding((CFStringRef)this->val);
	if (enc == kCFStringEncodingASCII)
	{
		return (const UTF8Char*)CFStringGetCStringPtr((CFStringRef)this->val, kCFStringEncodingASCII);
	}
	else
	{
		return (const UTF8Char*)CFStringGetCStringPtr((CFStringRef)this->val, kCFStringEncodingUTF8);
	}
}

void Data::MacString::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	const UTF8Char *s = this->Ptr();
	if (s)
	{
		Text::JSText::ToJSTextDQuote(sb, s);
	}
	else
	{
		CFIndex len = CFStringGetLength((CFStringRef)this->val);
		if (len > 0)
		{
			UniChar *buff = MemAlloc(UniChar, len + 1);
			CFStringGetCharacters((CFStringRef)this->val, CFRangeMake(0, len), buff);
			sb->AppendUTF8Char('"');
			sb->AppendUTF16((const UTF16Char*)buff);
			sb->AppendUTF8Char('"');
			MemFree(buff);
		}
		else
		{
			CFStringEncoding enc = CFStringGetFastestEncoding((CFStringRef)this->val);
			sb->AppendC(UTF8STRC("Unknown String Encoding ("));
			sb->AppendU32(enc);
			sb->AppendUTF8Char(')');
		}
	}
}

Data::MacNumber::MacNumber(CFNumberRef val) : MacType(val)
{
}

Data::MacNumber::~MacNumber()
{
}

Bool Data::MacNumber::IsFloat() const
{
	return CFNumberIsFloatType((CFNumberRef)this->val);
}
Double Data::MacNumber::GetDouble() const
{
	Double v;
	if (CFNumberGetValue((CFNumberRef)this->val, kCFNumberFloat64Type, &v))
		return v;
	return 0;
}

Int32 Data::MacNumber::GetInt32() const
{
	Int32 v;
	if (CFNumberGetValue((CFNumberRef)this->val, kCFNumberSInt32Type, &v))
		return v;
	return 0;
}

Int64 Data::MacNumber::GetInt64() const
{
	Int64 v;
	if (CFNumberGetValue((CFNumberRef)this->val, kCFNumberSInt64Type, &v))
		return v;
	return 0;
}

void Data::MacNumber::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	if (this->IsFloat())
	{
		sb->AppendDouble(this->GetDouble());
	}
	else
	{
		sb->AppendI64(this->GetInt64());
	}
}

Data::MacBoolean::MacBoolean(CFBooleanRef val) : MacType(val)
{
}

Data::MacBoolean::~MacBoolean()
{
}

Bool Data::MacBoolean::GetValue() const
{
	return CFBooleanGetValue((CFBooleanRef)this->val);
}

void Data::MacBoolean::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(this->GetValue()?CSTR("true"):CSTR("false"));
}

Data::MacData::MacData(CFDataRef val) : MacType(val)
{
}

Data::MacData::~MacData()
{
}

const UInt8 *Data::MacData::Ptr() const
{
	return CFDataGetBytePtr((CFDataRef)this->val);
}

UOSInt Data::MacData::GetSize() const
{
	return (UOSInt)CFDataGetLength((CFDataRef)this->val);
}

Data::ByteArrayR Data::MacData::GetArray() const
{
	return Data::ByteArrayR(this->Ptr(), this->GetSize());
}

void Data::MacData::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendHexBuff(this->GetArray(), ' ', Text::LineBreakType::CRLF);
}

Data::MacArray::MacArray(CFArrayRef val) : MacType(val)
{
}

Data::MacArray::~MacArray()
{
}

CFTypeRef Data::MacArray::GetItem(UOSInt i) const
{
	return CFArrayGetValueAtIndex((CFArrayRef)this->val, (CFIndex)i);
}

UOSInt Data::MacArray::GetCount() const
{
	return (UOSInt)CFArrayGetCount((CFArrayRef)this->val);
}

void Data::MacArray::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	sb->AppendUTF8Char('[');
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		MacType(this->GetItem(i)).ToString(sb);
		i++;
	}
	sb->AppendUTF8Char(']');
}

Data::MacDictionary::MacDictionary(CFDictionaryRef val) : MacType(val)
{
}

Data::MacDictionary::~MacDictionary()
{
}

CFTypeRef Data::MacDictionary::Get(Text::CString name) const
{
	CFStringRef s = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, (const char*)name.v, kCFStringEncodingUTF8, kCFAllocatorDefault);
	CFTypeRef v = CFDictionaryGetValue((CFDictionaryRef)this->val, s);
	CFRelease(s);
	return v;
}

CFTypeRef Data::MacDictionary::Get(CFStringRef name) const
{
	return CFDictionaryGetValue((CFDictionaryRef)this->val, name);
}

UOSInt Data::MacDictionary::GetCount() const
{
	return (UOSInt)CFDictionaryGetCount((CFDictionaryRef)this->val); 
}

void Data::MacDictionary::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i = 0;
	UOSInt cnt = this->GetCount();
	sb->AppendUTF8Char('{');
	if (cnt > 0)
	{
		CFTypeRef *keys = MemAlloc(CFTypeRef, cnt);
		CFTypeRef *vals = MemAlloc(CFTypeRef, cnt);
		CFDictionaryGetKeysAndValues((CFDictionaryRef)this->val, (const void**)keys, (const void**)vals);
		while (i < cnt)
		{
			if (i > 0)
			{
				sb->AppendC(UTF8STRC(",\r\n"));
			}
			MacType(keys[i]).ToString(sb);
			sb->AppendC(UTF8STRC(": "));
			MacType(vals[i]).ToString(sb);
			i++;
		}
		MemFree(keys);
		MemFree(vals);
	}
	sb->AppendUTF8Char('}');
}
