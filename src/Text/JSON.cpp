#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

Text::JSONBase::JSONBase()
{
	this->useCnt = 1;
}

Text::JSONBase::~JSONBase()
{
}

void Text::JSONBase::BeginUse()
{
	this->useCnt++;
}

void Text::JSONBase::EndUse()
{
	if (--this->useCnt == 0)
	{
		DEL_CLASS(this);
	}
}

Bool Text::JSONBase::IsString()
{
	return this->GetType() == JSONType::String;
}

Optional<Text::JSONBase> Text::JSONBase::GetValue(Text::CStringNN path)
{
	if (path.Equals(UTF8STRC("this")))
	{
		return this;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(path);
	UnsafeArray<UTF8Char> sptr = sb.v;
	UnsafeArray<UTF8Char> sptrEnd = sb.GetEndPtr();
	UIntOS dotIndex;
	UIntOS brkIndex;
	Text::JSONBase *json = this;
	while (json)
	{
		dotIndex = Text::StrIndexOfChar(sptr, '.');
		brkIndex = Text::StrIndexOfChar(sptr, '[');
		if (dotIndex == INVALID_INDEX && brkIndex == INVALID_INDEX)
		{
			if (json->GetType() == JSONType::Object)
			{
				return ((Text::JSONObject*)json)->GetObjectValue(CSTRP(sptr, sptrEnd));
			}
			else if (json->GetType() == JSONType::Array)
			{
				if (Text::StrToUIntOS(sptr, dotIndex))
				{
					return ((Text::JSONArray*)json)->GetArrayValue(dotIndex);
				}
			}
			return nullptr;
		}
		else
		{
			Bool isDot = false;
			if (brkIndex == INVALID_INDEX)
			{
				isDot = true;
			}
			else if (dotIndex == INVALID_INDEX)
			{
				dotIndex = brkIndex;
				isDot = false;
			}
			else if (dotIndex < brkIndex)
			{
				isDot = true;
			}
			else
			{
				dotIndex = brkIndex;
				isDot = false;
			}
			sptr[dotIndex] = 0;
			if (json->GetType() == JSONType::Object)
			{
				json = ((Text::JSONObject*)json)->GetObjectValue(Text::CStringNN(sptr, dotIndex)).OrNull();
			}
			else if (json->GetType() == JSONType::Array)
			{
				if (Text::StrToUIntOS(sptr, brkIndex))
				{
					json = ((Text::JSONArray*)json)->GetArrayValue(brkIndex).OrNull();
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
			sptr += dotIndex + 1;
			if (!isDot)
			{
				if (json == 0)
				{
					return nullptr;
				}
				dotIndex = Text::StrIndexOfChar(sptr, ']');
				if (dotIndex == INVALID_INDEX)
				{
					return nullptr;
				}
				if (json->GetType() != JSONType::Array)
				{
					return nullptr;
				}
				sptr[dotIndex] = 0;
				if (!Text::StrToUIntOS(sptr, brkIndex))
				{
					return nullptr;
				}
				json = ((Text::JSONArray*)json)->GetArrayValue(brkIndex).OrNull();
				sptr += dotIndex + 1;
				if (sptr[0] == 0)
				{
					return json;
				}
				else if (sptr[0] == '.')
				{
					sptr++;
				}
				else if (sptr[0] == '[')
				{

				}
				else
				{
					return nullptr;
				}
			}
		}
	}
	return nullptr;
}

Optional<Text::String> Text::JSONBase::GetValueString(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json) && json->IsString())
	{
		return NN<Text::JSONString>::ConvertFrom(json)->GetValue();
	}
	return nullptr;
}

Optional<Text::String> Text::JSONBase::GetValueNewString(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json) && json->IsString())
	{
		return NN<Text::JSONString>::ConvertFrom(json)->GetValue()->Clone();
	}
	return nullptr;
}

Int32 Text::JSONBase::GetValueAsInt32(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsInt32();
	}
	return 0;
}

Bool Text::JSONBase::GetValueAsInt32(Text::CStringNN path, OutParam<Int32> val)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsInt32(val);
	}
	return false;
}

Int64 Text::JSONBase::GetValueAsInt64(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsInt64();
	}
	return 0;
}

Bool Text::JSONBase::GetValueAsInt64(Text::CStringNN path, OutParam<Int64> val)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsInt64(val);
	}
	return false;
}

Bool Text::JSONBase::GetValueAsDouble(Text::CStringNN path, OutParam<Double> val)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsDouble(val);
	}
	return false;
}

Bool Text::JSONBase::GetValueAsBool(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetAsBool();
	}
	return false;
}

Optional<Text::JSONArray> Text::JSONBase::GetValueArray(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json) && json->GetType() == Text::JSONType::Array)
	{
		return NN<Text::JSONArray>::ConvertFrom(json);
	}
	return nullptr;
}

Optional<Text::JSONObject> Text::JSONBase::GetValueObject(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json) && json->GetType() == Text::JSONType::Object)
	{
		return NN<Text::JSONObject>::ConvertFrom(json);
	}
	return nullptr;
}

Text::JSONType Text::JSONBase::GetValueType(Text::CStringNN path)
{
	NN<Text::JSONBase> json;
	if (this->GetValue(path).SetTo(json))
	{
		return json->GetType();
	}
	return Text::JSONType::Null;
}

Int32 Text::JSONBase::GetAsInt32()
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		return ((Text::JSONBool*)this)->GetValue()?1:0;
	case Text::JSONType::INT32:
		return ((Text::JSONInt32*)this)->GetValue();
	case Text::JSONType::INT64:
		return (Int32)(((Text::JSONInt64*)this)->GetValue());
	case Text::JSONType::Number:
		return Double2Int32(((Text::JSONNumber*)this)->GetValue());
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToInt32();
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return 0;
	}
	return 0;
}

Bool Text::JSONBase::GetAsInt32(OutParam<Int32> val)
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		val.Set(((Text::JSONBool*)this)->GetValue()?1:0);
		return true;
	case Text::JSONType::INT32:
		val.Set(((Text::JSONInt32*)this)->GetValue());
		return true;
	case Text::JSONType::INT64:
		{
			Int32 iv = (Int32)(((Text::JSONInt64*)this)->GetValue());
			val.Set(iv);
			return iv == ((Text::JSONInt64*)this)->GetValue();
		}
	case Text::JSONType::Number:
		{
			Int32 iv = Double2Int32(((Text::JSONNumber*)this)->GetValue());
			val.Set(iv);
			return iv == ((Text::JSONNumber*)this)->GetValue();
		}
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToInt32(val);
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return false;
	}
	return false;
}

Int64 Text::JSONBase::GetAsInt64()
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		return ((Text::JSONBool*)this)->GetValue()?1:0;
	case Text::JSONType::INT32:
		return ((Text::JSONInt32*)this)->GetValue();
	case Text::JSONType::INT64:
		return ((Text::JSONInt64*)this)->GetValue();
	case Text::JSONType::Number:
		return Double2Int64(((Text::JSONNumber*)this)->GetValue());
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToInt64();
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return 0;
	}
	return 0;
}

Bool Text::JSONBase::GetAsInt64(OutParam<Int64> val)
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		val.Set(((Text::JSONBool*)this)->GetValue()?1:0);
		return true;
	case Text::JSONType::INT32:
		val.Set(((Text::JSONInt32*)this)->GetValue());
		return true;
	case Text::JSONType::INT64:
		val.Set(((Text::JSONInt64*)this)->GetValue());
		return true;
	case Text::JSONType::Number:
		{
			Int64 iv = Double2Int64(((Text::JSONNumber*)this)->GetValue());
			val.Set(iv);
			return (Double)iv == ((Text::JSONNumber*)this)->GetValue();
		}
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToInt64(val);
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return false;
	}
	return false;
}

Double Text::JSONBase::GetAsDoubleOrNAN()
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		return ((Text::JSONBool*)this)->GetValue()?1:0;
	case Text::JSONType::INT32:
		return ((Text::JSONInt32*)this)->GetValue();
	case Text::JSONType::INT64:
		return (Double)(((Text::JSONInt64*)this)->GetValue());
	case Text::JSONType::Number:
		return ((Text::JSONNumber*)this)->GetValue();
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToDoubleOrNAN();
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return NAN;
	}
	return NAN;
}

Double Text::JSONBase::GetAsDoubleOr(Double v)
{
	return Math::NANTo(this->GetAsDoubleOrNAN(), v);
}

Bool Text::JSONBase::GetAsDouble(OutParam<Double> val)
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		val.Set(((Text::JSONBool*)this)->GetValue()?1:0);
		return true;
	case Text::JSONType::INT32:
		val.Set(((Text::JSONInt32*)this)->GetValue());
		return true;
	case Text::JSONType::INT64:
		val.Set((Double)(((Text::JSONInt64*)this)->GetValue()));
		return true;
	case Text::JSONType::Number:
		val.Set(((Text::JSONNumber*)this)->GetValue());
		return true;
	case Text::JSONType::String:
		return ((Text::JSONString*)this)->GetValue()->ToDouble(val);
	case Text::JSONType::Array:
	case Text::JSONType::Object:
	case Text::JSONType::Null:
		return false;
	}
	return 0;
}

Bool Text::JSONBase::GetAsBool()
{
	switch (this->GetType())
	{
	case Text::JSONType::BOOL:
		return ((Text::JSONBool*)this)->GetValue();
	case Text::JSONType::INT32:
		return ((Text::JSONInt32*)this)->GetValue() != 0;
	case Text::JSONType::INT64:
		return (Double)(((Text::JSONInt64*)this)->GetValue()) != 0;
	case Text::JSONType::Number:
		return ((Text::JSONNumber*)this)->GetValue() != 0;
	case Text::JSONType::String:
		return Str2Bool(((Text::JSONString*)this)->GetValue());
	case Text::JSONType::Array:
	case Text::JSONType::Object:
		return true;
	case Text::JSONType::Null:
		return 0;
	}
	return 0;
}

Optional<Text::JSONBase> Text::JSONBase::ParseJSONStr(Text::CStringNN jsonStr)
{
	UnsafeArrayOpt<const UTF8Char> endPtr;
	Text::StringBuilderUTF8 sbEnv;
	return ParseJSONStr2(jsonStr.v, jsonStr.GetEndPtr(), endPtr, sbEnv);
}

Optional<Text::JSONBase> Text::JSONBase::ParseJSONBytes(UnsafeArray<const UInt8> jsonBytes, UIntOS len)
{
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, len + 1);
	UnsafeArrayOpt<const UTF8Char> endPtr;
	Text::StringBuilderUTF8 sbEnv;
	Optional<Text::JSONBase> ret = ParseJSONStr2(s, Text::StrConcatC(s, jsonBytes, len), endPtr, sbEnv);
	MemFreeArr(s);
	return ret;
}

Optional<Text::JSONBase> Text::JSONBase::ParseJSONBytes(const Data::ByteArrayR &jsonBytes)
{
	UnsafeArray<UTF8Char> s = MemAllocArr(UTF8Char, jsonBytes.GetSize() + 1);
	UnsafeArrayOpt<const UTF8Char> endPtr;
	Text::StringBuilderUTF8 sbEnv;
	Optional<Text::JSONBase> ret = ParseJSONStr2(s, Text::StrConcatC(s, jsonBytes.Arr(), jsonBytes.GetSize()), endPtr, sbEnv);
	MemFreeArr(s);
	return ret;
}

UnsafeArray<const UTF8Char> Text::JSONBase::ClearWS(UnsafeArray<const UTF8Char> jsonStr)
{
	UTF8Char c;
	while (true)
	{
		c = *jsonStr;
		if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
		{
			jsonStr++;
		}
		else
		{
			break;
		}
	}
	return jsonStr;
}

UnsafeArrayOpt<const UTF8Char> Text::JSONBase::ParseJSString(UnsafeArray<const UTF8Char> jsonStr, NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char c;
	c = *jsonStr++;
	if (c != '\"')
		return nullptr;
	sptr = sbuff;
	while (true)
	{
		c = *jsonStr++;
		if (c == '\"')
		{
			if (sptr != sbuff)
			{
				*sptr = 0;
				sb->AppendC(sbuff, (UIntOS)(sptr - sbuff));
			}
			return jsonStr;
		}
		else if (c == '\\')
		{
			c = *jsonStr++;
			if (c == '\"')
			{
				*sptr++ = '\"';
			}
			else if (c == '\\')
			{
				*sptr++ = '\\';
			}
			else if (c == '/')
			{
				*sptr++ = '/';
			}
			else if (c == 'b')
			{
				*sptr++ = '\b';
			}
			else if (c == 'r')
			{
				*sptr++ = '\r';
			}
			else if (c == 'n')
			{
				*sptr++ = '\n';
			}
			else if (c == 'f')
			{
				*sptr++ = '\f';
			}
			else if (c == 't')
			{
				*sptr++ = '\t';
			}
			else if (c == 'u')
			{
				UInt32 v = 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return nullptr;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return nullptr;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return nullptr;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return nullptr;

				if (v < 0x80)
				{
					*sptr++ = (UInt8)v;
				}
				else if (v < 0x800)
				{
					*sptr++ = (UTF8Char)(0xc0 | (v >> 6));
					*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
				}
				else if (v >= 0xd800 && v < 0xdc00)
				{
					if (jsonStr[0] == '\\' && jsonStr[1] == 'u')
					{
						UInt32 v2 = 0;
						jsonStr += 2;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return nullptr;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return nullptr;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return nullptr;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return nullptr;

						if (v2 >= 0xdc00 && v2 < 0xe000)
						{
							v = 0x10000 + ((v - 0xd800) << 10) + (v2 - 0xdc00);
							if (v < 0x200000)
							{
								*sptr++ = (UTF8Char)(0xf0 | (v >> 18));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 12) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
							}
							else if (v < 0x4000000)
							{
								*sptr++ = (UTF8Char)(0xf8 | (v >> 24));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 18) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 12) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
							}
							else
							{
								*sptr++ = (UTF8Char)(0xfc | (v >> 30));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 24) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 18) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 12) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
								*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
							}
						}
						else
						{
							*sptr++ = (UTF8Char)(0xe0 | (v >> 12));
							*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
							*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
							*sptr++ = (UTF8Char)(0xe0 | (v2 >> 12));
							*sptr++ = (UTF8Char)(0x80 | ((v2 >> 6) & 0x3f));
							*sptr++ = (UTF8Char)(0x80 | (v2 & 0x3f));
						}
					}
					else
					{
						*sptr++ = (UTF8Char)(0xe0 | (v >> 12));
						*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
						*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
					}
				}
				else
				{
					*sptr++ = (UTF8Char)(0xe0 | (v >> 12));
					*sptr++ = (UTF8Char)(0x80 | ((v >> 6) & 0x3f));
					*sptr++ = (UTF8Char)(0x80 | (v & 0x3f));
				}
			}
			else
			{
				return nullptr;
			}
		}
		else if (c == 0)
		{
			return nullptr;
		}
		else
		{
			*sptr++ = c;
		}
		if ((sptr - sbuff) >= 126)
		{
			*sptr = 0;
			sb->AppendC(sbuff, (UIntOS)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	return nullptr;
}

UnsafeArrayOpt<const UTF8Char> Text::JSONBase::ParseJSNumber(UnsafeArray<const UTF8Char> jsonStr, OutParam<Double> val, OptOut<Bool> noDecimal)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> dptr = sbuff;
	UTF8Char c;
	Bool hasDot = false;
	Bool hasE = false;
	Bool numStart = true;
	while (true)
	{
		c = *jsonStr;
		if (c >= '0' && c <= '9')
		{
			*dptr++ = c;
			numStart = false;
		}
		else if (c == '-')
		{
			if (!numStart)
				return nullptr;
			*dptr++ = c;
			numStart = false;

		}
		else if (c == '.')
		{
			if (hasDot || hasE)
				return nullptr;
			hasDot = true;
			numStart = false;
			*dptr++ = c;
		}
		else if (c == 'e' || c == 'E')
		{
			if (hasE || numStart)
				return nullptr;
			hasE = true;
			*dptr++ = c;
			numStart = true;
		}
		else if (c == '+' && hasE && numStart)
		{
			numStart = false;
			*dptr++ = c;
		}
		else
		{
			*dptr = 0;
			val.Set(Text::StrToDoubleOrNAN(sbuff));
			noDecimal.Set(!hasDot);
			return jsonStr;
		}
		jsonStr++;
	}
}


Optional<Text::JSONBase> Text::JSONBase::ParseJSONStr2(UnsafeArray<const UTF8Char> jsonStr, UnsafeArray<const UTF8Char> jsonStrEnd, OutParam<UnsafeArrayOpt<const UTF8Char>> jsonStrEndOut, NN<Text::StringBuilderUTF8> sbEnv)
{
	UTF8Char c;
	UnsafeArrayOpt<const UTF8Char> optStr;
	jsonStr = ClearWS(jsonStr);
	c = *jsonStr;
	if (c == 0)
	{
		jsonStrEndOut.Set(nullptr);
		return nullptr;
	}
	else if (c == '{')
	{
		Text::JSONObject *jobj;

		jsonStr++;
		NEW_CLASS(jobj, Text::JSONObject());

		jsonStr = ClearWS(jsonStr);
		c = *jsonStr;
		if (c == '}')
		{
			jsonStr++;
			jsonStrEndOut.Set(jsonStr);
			return jobj;
		}
		Text::StringBuilderUTF8 sb;
		while (true)
		{
			jsonStr = ClearWS(jsonStr);
			c = *jsonStr;
			if (c == '\"')
			{
				Optional<Text::JSONBase> obj;
				NN<Text::JSONBase> nnobj;
				sb.ClearStr();
				if (!ParseJSString(jsonStr, sb).SetTo(jsonStr))
				{
					jsonStrEndOut.Set(nullptr);
					jobj->EndUse();
					return nullptr;
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c != ':')
				{
					jsonStrEndOut.Set(nullptr);
					jobj->EndUse();
					return nullptr;
				}
				jsonStr++;
				jsonStr = ClearWS(jsonStr);

				obj = ParseJSONStr2(jsonStr, jsonStrEnd, optStr, sbEnv);
				if (!optStr.SetTo(jsonStr))
				{
					jobj->EndUse();
					jsonStrEndOut.Set(nullptr);
					return nullptr;
				}
				jobj->SetObjectValue(sb.ToCString(), obj);
				if (obj.SetTo(nnobj))
				{
					nnobj->EndUse();
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c == '}')
				{
					jsonStr++;
					jsonStrEndOut.Set(jsonStr);
					return jobj;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					jobj->EndUse();
					jsonStrEndOut.Set(nullptr);
					return nullptr;
				}
			}
			else
			{
				jsonStrEndOut.Set(nullptr);
				jobj->EndUse();
				return nullptr;
			}
		}
	}
	else if (c == '[')
	{
		Text::JSONArray *arr;
		Optional<Text::JSONBase> obj;
		NN<Text::JSONBase> nnobj;

		jsonStr++;
		NEW_CLASS(arr, Text::JSONArray());
		jsonStr = ClearWS(jsonStr);
		if (*jsonStr == ']')
		{
			jsonStr++;
			jsonStrEndOut.Set(jsonStr);
			return arr;
		}
		while (true)
		{
			c = *jsonStr;
			if (c == 0)
			{
				arr->EndUse();
				jsonStrEndOut.Set(nullptr);
				return nullptr;
			}
			else
			{
				obj = ParseJSONStr2(jsonStr, jsonStrEnd, optStr, sbEnv);
				if (!optStr.SetTo(jsonStr))
				{
					arr->EndUse();
					jsonStrEndOut.Set(nullptr);
					return nullptr;
				}
				arr->AddArrayValue(obj);
				if (obj.SetTo(nnobj))
				{
					nnobj->EndUse();
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c == ']')
				{
					jsonStr++;
					jsonStrEndOut.Set(jsonStr);
					return arr;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					arr->EndUse();
					jsonStrEndOut.Set(nullptr);
					return nullptr;
				}
			}
		}
		return nullptr;
	}
	else if (c == '\"')
	{
		UnsafeArray<const UTF8Char> endPtr;
		sbEnv->ClearStr();
		if (!ParseJSString(jsonStr, sbEnv).SetTo(endPtr))
		{
			jsonStrEndOut.Set(nullptr);
			return nullptr;
		}
		Text::JSONString *s;
		jsonStrEndOut.Set(endPtr);
		NEW_CLASS(s, Text::JSONString(sbEnv->ToCString()));
		return s;
	}
	else if (c == '-' || (c >= '0' && c <= '9'))
	{
		Double val;
		Bool noDecimal;
		if (!ParseJSNumber(jsonStr, val, noDecimal).SetTo(jsonStr))
		{
			jsonStrEndOut.Set(nullptr);
			return nullptr;
		}
		else 
		{
			if (noDecimal && val >= -0x80000000 && val <= 0x7fffffff)
			{
				Text::JSONInt32 *num;
				NEW_CLASS(num, Text::JSONInt32(Double2Int32(val)));
				jsonStrEndOut.Set(jsonStr);
				return num;
			}
			else if (noDecimal && val >= -0x8000000000000000LL && (Int64)val <= 0x7fffffffffffffffLL)
			{
				Text::JSONInt64 *num;
				NEW_CLASS(num, Text::JSONInt64((Int64)val));
				jsonStrEndOut.Set(jsonStr);
				return num;
			}
			else
			{
				Text::JSONNumber *num;
				NEW_CLASS(num, Text::JSONNumber(val));
				jsonStrEndOut.Set(jsonStr);
				return num;
			}
		}
	}
	else if (c == 't')
	{
		if (Text::StrStartsWithC(jsonStr, (UIntOS)(jsonStrEnd - jsonStr), UTF8STRC("true")))
		{
			Text::JSONBool *b;
			jsonStrEndOut.Set(&jsonStr[4]);
			NEW_CLASS(b, Text::JSONBool(true));
			return b;
		}
		else
		{
			jsonStrEndOut.Set(nullptr);
			return nullptr;
		}
	}
	else if (c == 'f')
	{
		if (Text::StrStartsWithC(jsonStr, (UIntOS)(jsonStrEnd - jsonStr), UTF8STRC("false")))
		{
			Text::JSONBool *b;
			jsonStrEndOut.Set(&jsonStr[5]);
			NEW_CLASS(b, Text::JSONBool(false));
			return b;
		}
		else
		{
			jsonStrEndOut.Set(nullptr);
			return nullptr;
		}
	}
	else if (c == 'n')
	{
		if (Text::StrStartsWithC(jsonStr, (UIntOS)(jsonStrEnd - jsonStr), UTF8STRC("null")))
		{
			Text::JSONNull *n;
			jsonStrEndOut.Set(&jsonStr[4]);
			NEW_CLASS(n, Text::JSONNull());
			return n;
		}
		else
		{
			jsonStrEndOut.Set(nullptr);
			return nullptr;
		}
	}
	else
	{
		jsonStrEndOut.Set(nullptr);
		return nullptr;
	}
}

Bool Text::JSONBase::Str2Bool(NN<Text::String> s)
{
	Double d;
	if (s->leng == 0)
		return false;
	if (s->ToDouble(d))
		return d != 0;
	if (s->Equals(CSTR("false")))
		return false;
	return true;
}

Text::JSONNumber::JSONNumber(Double val)
{
	this->val = val;
}

Text::JSONNumber::~JSONNumber()
{
}

Text::JSONType Text::JSONNumber::GetType()
{
	return Text::JSONType::Number;
}

void Text::JSONNumber::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	Text::SBAppendF64(sb, this->val);
}

void Text::JSONNumber::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	Text::SBAppendF64(sb, this->val);
}

Bool Text::JSONNumber::Equals(Text::CStringNN s)
{
	return false;
}

Bool Text::JSONNumber::Identical(NN<Text::JSONBase> obj)
{
	if (obj->GetType() != Text::JSONType::Number)
		return false;
	return ((Text::JSONNumber*)obj.Ptr())->GetValue() == this->val;
}

void Text::JSONNumber::ToString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendDouble(this->val);
}

Double Text::JSONNumber::GetValue()
{
	return this->val;
}

Text::JSONInt32::JSONInt32(Int32 val)
{
	this->val = val;
}

Text::JSONInt32::~JSONInt32()
{
}

Text::JSONType Text::JSONInt32::GetType()
{
	return Text::JSONType::INT32;
}

void Text::JSONInt32::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendI32(this->val);
}

void Text::JSONInt32::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	sb->AppendI32(this->val);
}

Bool Text::JSONInt32::Equals(Text::CStringNN s)
{
	return false;
}

Bool Text::JSONInt32::Identical(NN<Text::JSONBase> obj)
{
	if (obj->GetType() != Text::JSONType::INT32)
		return false;
	return ((Text::JSONInt32*)obj.Ptr())->GetValue() == this->val;
}

void Text::JSONInt32::ToString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendI32(this->val);
}

Int32 Text::JSONInt32::GetValue()
{
	return this->val;
}

Text::JSONInt64::JSONInt64(Int64 val)
{
	this->val = val;
}

Text::JSONInt64::~JSONInt64()
{
}

Text::JSONType Text::JSONInt64::GetType()
{
	return Text::JSONType::INT64;
}

void Text::JSONInt64::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendI64(this->val);
}

void Text::JSONInt64::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	sb->AppendI64(this->val);
}

Bool Text::JSONInt64::Equals(Text::CStringNN s)
{
	return false;
}

Bool Text::JSONInt64::Identical(NN<Text::JSONBase> obj)
{
	if (obj->GetType() != Text::JSONType::INT64)
		return false;
	return ((Text::JSONInt64*)obj.Ptr())->GetValue() == this->val;
}

void Text::JSONInt64::ToString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendI64(this->val);
}

Int64 Text::JSONInt64::GetValue()
{
	return this->val;
}

Text::JSONString::JSONString(NN<Text::String> val)
{
	this->val = val->Clone();
}

Text::JSONString::JSONString(Text::CStringNN val)
{
	this->val = Text::String::New(val);
}

Text::JSONString::~JSONString()
{
	this->val->Release();
}

Text::JSONType Text::JSONString::GetType()
{
	return Text::JSONType::String;
}

void Text::JSONString::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[128];
	UnsafeArray<const UTF8Char> sptr;
	UnsafeArray<UTF8Char> dptr;
	UTF8Char c;
	sb->AppendUTF8Char('\"');
	sptr = this->val->v;
	dptr = sbuff;
	while ((c = *sptr++) != 0)
	{
		if (c == '\"')
		{
			*dptr++ = '\\';
			*dptr++ = '\"';
		}
		else if (c == '\r')
		{
			*dptr++ = '\\';
			*dptr++ = 'r';
		}
		else if (c == '\n')
		{
			*dptr++ = '\\';
			*dptr++ = 'n';
		}
		else
		{
			*dptr++ = c;
		}
		if (dptr - sbuff >= 126)
		{
			sb->AppendC(sbuff,(UIntOS)(dptr - sbuff));
			dptr = sbuff;
		}
	}
	if (dptr - sbuff > 0)
	{
		sb->AppendC(sbuff, (UIntOS)(dptr - sbuff));
	}
	sb->AppendUTF8Char('\"');
}

void Text::JSONString::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	this->ToJSONString(sb);
}

Bool Text::JSONString::Equals(Text::CStringNN s)
{
	return this->val->Equals(s.v, s.leng);
}

Bool Text::JSONString::Identical(NN<Text::JSONBase> obj)
{
	if (obj->GetType() != Text::JSONType::String)
		return false;
	return Text::StringTool::Equals(((Text::JSONString*)obj.Ptr())->GetValue(), this->val);
}

void Text::JSONString::ToString(NN<Text::StringBuilderUTF8> sb)
{
	sb->Append(this->val);
}

NN<Text::String> Text::JSONString::GetValue()
{
	return this->val;
}

Text::JSONBool::JSONBool(Bool val)
{
	this->val = val;
}

Text::JSONBool::~JSONBool()
{
}

Text::JSONType Text::JSONBool::GetType()
{
	return Text::JSONType::BOOL;
}

void Text::JSONBool::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	if (this->val)
	{
		sb->AppendC(UTF8STRC("true"));
	}
	else
	{
		sb->AppendC(UTF8STRC("false"));
	}
}

void Text::JSONBool::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	this->ToJSONString(sb);
}

Bool Text::JSONBool::Equals(Text::CStringNN s)
{
	if  (this->val)
		return s.EqualsICase(UTF8STRC("true"));
	else
		return s.EqualsICase(UTF8STRC("false"));
}

Bool Text::JSONBool::Identical(NN<Text::JSONBase> obj)
{
	if (obj->GetType() != Text::JSONType::BOOL)
		return false;
	return ((Text::JSONBool*)obj.Ptr())->GetValue() == val;
}

void Text::JSONBool::ToString(NN<Text::StringBuilderUTF8> sb)
{
	this->ToJSONString(sb);
}

Bool Text::JSONBool::GetValue()
{
	return this->val;
}

Text::JSONObject::JSONObject()
{
}

Text::JSONObject::~JSONObject()
{
	UIntOS i;
	NN<Text::JSONBase> obj;
	i = this->objVals.GetCount();
	while (i-- > 0)
	{
		if (this->objVals.GetItem(i).SetTo(obj))
			obj->EndUse();
	}
}

Text::JSONType Text::JSONObject::GetType()
{
	return Text::JSONType::Object;
}

void Text::JSONObject::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::JSONBase> obj;
	UIntOS i = 0;
	UIntOS j = this->objVals.GetCount();
	sb->AppendUTF8Char('{');
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		Text::JSText::ToJSTextDQuote(sb, Text::String::OrEmpty(this->objVals.GetKey(i))->v);
		sb->AppendC(UTF8STRC(" : "));
		if (this->objVals.GetItem(i).SetTo(obj))
		{
			obj->ToJSONString(sb);
		}
		else
		{
			sb->AppendC(UTF8STRC("null"));
		}
		i++;
	}
	sb->AppendUTF8Char('}');
}

void Text::JSONObject::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	NN<Text::JSONBase> obj;
	UIntOS i = 0;
	UIntOS j = this->objVals.GetCount();
	sb->AppendUTF8Char('{');
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendChar('\t', level + 1);
		Text::JSText::ToJSTextDQuote(sb, Text::String::OrEmpty(this->objVals.GetKey(i))->v);
		sb->AppendC(UTF8STRC(" : "));
		if (this->objVals.GetItem(i).SetTo(obj))
		{
			obj->ToJSONStringWF(sb, level + 1);
		}
		else
		{
			sb->AppendC(UTF8STRC("null"));
		}
		i++;
	}
	if (j > 0)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendChar('\t', level);
	}
	sb->AppendUTF8Char('}');
}

Bool Text::JSONObject::Equals(Text::CStringNN s)
{
	///////////////////////////////
	return false;
}

Bool Text::JSONObject::Identical(NN<Text::JSONBase> obj)
{
	return this == obj.Ptr();
}

void Text::JSONObject::ToString(NN<Text::StringBuilderUTF8> sb)
{
	this->ToJSONString(sb);
}

NN<Text::JSONObject> Text::JSONObject::SetObjectValue(Text::CStringNN name, Optional<Text::JSONBase> val)
{
	NN<Text::JSONBase> nnobj;
	if (val.SetTo(nnobj))
	{
		nnobj->BeginUse();
	}
	if (this->objVals.RemoveC(name).SetTo(nnobj))
	{
		nnobj->EndUse();
	}
	if (val.SetTo(nnobj))
		this->objVals.PutC(name, nnobj);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectValueAndRelease(Text::CStringNN name, NN<Text::JSONBase> val)
{
	NN<Text::JSONBase> nnobj;
	if (this->objVals.PutC(name, val).SetTo(nnobj))
	{
		nnobj->EndUse();
	}
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectInt32(Text::CStringNN name, Int32 val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONInt32> ival;
	NEW_CLASSNN(ival, Text::JSONInt32(val));
	this->objVals.PutC(name, ival);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectInt64(Text::CStringNN name, Int64 val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONInt64> ival;
	NEW_CLASSNN(ival, Text::JSONInt64(val));
	this->objVals.PutC(name, ival);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectDouble(Text::CStringNN name, Double val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONNumber> ival;
	NEW_CLASSNN(ival, Text::JSONNumber(val));
	this->objVals.PutC(name, ival);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectString(Text::CStringNN name, Text::CStringNN val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONString> ival;
	NEW_CLASSNN(ival, Text::JSONString(val));
	this->objVals.PutC(name, ival);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectString(Text::CStringNN name, Optional<Text::String> val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::String> s;
	if (val.SetTo(s))
	{
		NN<Text::JSONString> ival;
		NEW_CLASSNN(ival, Text::JSONString(s));
		this->objVals.PutC(name, ival);
	}
	else
	{
		NN<Text::JSONNull> ival;
		NEW_CLASSNN(ival, Text::JSONNull());
		this->objVals.PutC(name, ival);
	}
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectString(Text::CStringNN name, NN<Text::String> val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONString> ival;
	NEW_CLASSNN(ival, Text::JSONString(val));
	this->objVals.PutC(name, ival);
	return *this;
}

NN<Text::JSONObject> Text::JSONObject::SetObjectBool(Text::CStringNN name, Bool val)
{
	NN<Text::JSONBase> obj;
	if (this->objVals.GetC(name).SetTo(obj))
	{
		obj->EndUse();
	}
	NN<Text::JSONBool> ival;
	NEW_CLASSNN(ival, Text::JSONBool(val));
	this->objVals.PutC(name, ival);
	return *this;
}

Optional<Text::JSONBase> Text::JSONObject::GetObjectValue(Text::CStringNN name)
{
	return this->objVals.GetC(name);
}

Optional<Text::JSONArray> Text::JSONObject::GetObjectArray(Text::CStringNN name)
{
	NN<Text::JSONBase> o;
	if (this->GetObjectValue(name).SetTo(o) && o->GetType() == Text::JSONType::Array)
		return NN<Text::JSONArray>::ConvertFrom(o);
	return nullptr;
}

Optional<Text::JSONObject> Text::JSONObject::GetObjectObject(Text::CStringNN name)
{
	NN<Text::JSONBase> o;
	if (this->GetObjectValue(name).SetTo(o) && o->GetType() == Text::JSONType::Object)
		return NN<Text::JSONObject>::ConvertFrom(o);
	return nullptr;
}

void Text::JSONObject::GetObjectNames(NN<Data::ArrayListNN<Text::String>> names)
{
	Data::FastStringNNKeyIterator<Text::JSONBase> it = this->objVals.KeyIterator();
	while (it.HasNext())
	{
		names->Add(it.Next());
	}
}

Optional<Text::String> Text::JSONObject::GetObjectString(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj) || baseObj->GetType() != Text::JSONType::String)
	{
		return nullptr;
	}
	return NN<Text::JSONString>::ConvertFrom(baseObj)->GetValue();
}

Optional<Text::String> Text::JSONObject::GetObjectNewString(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj) || baseObj->GetType() != Text::JSONType::String)
	{
		return nullptr;
	}
	return NN<Text::JSONString>::ConvertFrom(baseObj)->GetValue()->Clone();
}

Double Text::JSONObject::GetObjectDoubleOrNAN(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return NAN;
	}
	return baseObj->GetAsDoubleOrNAN();
}

Double Text::JSONObject::GetObjectDoubleOr(Text::CStringNN name, Double v)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return v;
	}
	return baseObj->GetAsDoubleOr(v);
}

Int32 Text::JSONObject::GetObjectInt32(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return 0;
	}
	return baseObj->GetAsInt32();
}

Int64 Text::JSONObject::GetObjectInt64(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return 0;
	}
	return baseObj->GetAsInt64();
}

Bool Text::JSONObject::GetObjectInt64(Text::CStringNN name, OutParam<Int64> v)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return false;
	}
	return baseObj->GetAsInt64(v);
}

Bool Text::JSONObject::GetObjectBool(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return false;
	}
	return baseObj->GetAsBool();
}

Bool Text::JSONObject::GetObjectBool(Text::CStringNN name, OutParam<Bool> v)
{
	NN<Text::JSONBase> baseObj;
	if (!this->objVals.GetC(name).SetTo(baseObj))
	{
		return false;
	}
	v.Set(baseObj->GetAsBool());
	return true;
}

void Text::JSONObject::RemoveObject(Text::CStringNN name)
{
	NN<Text::JSONBase> baseObj;
	if (this->objVals.RemoveC(name).SetTo(baseObj))
	{
		baseObj->EndUse();
	}
}

NN<Text::JSONObject> Text::JSONObject::New()
{
	NN<Text::JSONObject> ret;
	NEW_CLASSNN(ret, Text::JSONObject());
	return ret;
}

Text::JSONArray::JSONArray()
{
}

Text::JSONArray::~JSONArray()
{
	NN<Text::JSONBase> obj;
	UIntOS i = this->arrVals.GetCount();
	while (i-- > 0)
	{
		if (this->arrVals.GetItem(i).SetTo(obj))
			obj->EndUse();
	}
}

Text::JSONType Text::JSONArray::GetType()
{
	return Text::JSONType::Array;
}

void Text::JSONArray::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::JSONBase> obj;
	UIntOS i = 0;
	UIntOS j = this->arrVals.GetCount();
	sb->AppendUTF8Char('[');
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		if (this->arrVals.GetItem(i).SetTo(obj))
		{
			obj->ToJSONString(sb);
		}
		else
		{
			sb->AppendC(UTF8STRC("null"));
		}
		i++;
	}
	sb->AppendUTF8Char(']');
}

void Text::JSONArray::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	NN<Text::JSONBase> obj;
	UIntOS i = 0;
	UIntOS j = this->arrVals.GetCount();
	sb->AppendUTF8Char('[');
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendChar('\t', level + 1);
		if (this->arrVals.GetItem(i).SetTo(obj))
		{
			obj->ToJSONStringWF(sb, level + 1);
		}
		else
		{
			sb->AppendC(UTF8STRC("null"));
		}
		i++;
	}
	if (j > 0)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendChar('\t', level);
	}
	sb->AppendUTF8Char(']');
}

Bool Text::JSONArray::Equals(Text::CStringNN s)
{
	///////////////////////////////
	return false;
}

Bool Text::JSONArray::Identical(NN<Text::JSONBase> obj)
{
	return this == obj.Ptr();
}

void Text::JSONArray::ToString(NN<Text::StringBuilderUTF8> sb)
{
	this->ToJSONString(sb);
}

void Text::JSONArray::SetArrayValue(UIntOS index, Optional<Text::JSONBase> val)
{
	NN<Text::JSONBase> nnobj;
	if (val.SetTo(nnobj))
	{
		nnobj->BeginUse();
	}
	if (this->arrVals.GetItem(index).SetTo(nnobj))
	{
		nnobj->EndUse();
	}
	this->arrVals.SetItem(index, val);
	////////////////////////////////////
}

void Text::JSONArray::AddArrayValue(Optional<Text::JSONBase> val)
{
	NN<Text::JSONBase> nnval;
	if (val.SetTo(nnval))
	{
		nnval->BeginUse();
	}
	this->arrVals.Add(val);
}

NN<Text::JSONArray> Text::JSONArray::AddArrayString(Text::CStringNN val)
{
	NN<Text::JSONString> str;
	NEW_CLASSNN(str, Text::JSONString(val));
	this->arrVals.Add(str);
	return *this;
}

Text::JSONType Text::JSONArray::GetArrayType(UIntOS index)
{
	NN<Text::JSONBase> o;
	if (this->arrVals.GetItem(index).SetTo(o))
		return o->GetType();
	return JSONType::Null;
}

Optional<Text::JSONBase> Text::JSONArray::GetArrayValue(UIntOS index)
{
	return this->arrVals.GetItem(index);
}

Optional<Text::JSONObject> Text::JSONArray::GetArrayObject(UIntOS index)
{
	NN<Text::JSONBase> o;
	if (this->GetArrayValue(index).SetTo(o) && o->GetType() == Text::JSONType::Object)
		return NN<Text::JSONObject>::ConvertFrom(o);
	return nullptr;
}

Double Text::JSONArray::GetArrayDoubleOrNAN(UIntOS index)
{
	NN<Text::JSONBase> baseObj;
	if (!this->arrVals.GetItem(index).SetTo(baseObj))
	{
		return NAN;
	}
	return baseObj->GetAsDoubleOrNAN();
}

Double Text::JSONArray::GetArrayDoubleOr(UIntOS index, Double v)
{
	NN<Text::JSONBase> baseObj;
	if (!this->arrVals.GetItem(index).SetTo(baseObj))
	{
		return v;
	}
	return baseObj->GetAsDoubleOr(v);
}

Optional<Text::String> Text::JSONArray::GetArrayString(UIntOS index)
{
	NN<Text::JSONBase> baseObj;
	if (!this->arrVals.GetItem(index).SetTo(baseObj) || baseObj->GetType() != Text::JSONType::String)
	{
		return nullptr;
	}
	return NN<Text::JSONString>::ConvertFrom(baseObj)->GetValue();
}

UIntOS Text::JSONArray::GetArrayLength()
{
	return this->arrVals.GetCount();
}

void Text::JSONArray::RemoveArrayItem(UIntOS index)
{
	NN<Text::JSONBase> baseObj;
	if (this->arrVals.RemoveAt(index).SetTo(baseObj))
	{
		baseObj->EndUse();
	}
}

NN<Text::JSONArray> Text::JSONArray::New()
{
	NN<Text::JSONArray> ret;
	NEW_CLASSNN(ret, Text::JSONArray());
	return ret;
}

Text::JSONNull::JSONNull()
{
}

Text::JSONNull::~JSONNull()
{
}

Text::JSONType Text::JSONNull::GetType()
{
	return Text::JSONType::Null;
}

void Text::JSONNull::ToJSONString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("null"));
}

void Text::JSONNull::ToJSONStringWF(NN<Text::StringBuilderUTF8> sb, UIntOS level)
{
	sb->AppendC(UTF8STRC("null"));
}

Bool Text::JSONNull::Equals(Text::CStringNN s)
{
	return s.Equals(UTF8STRC("null"));
}

Bool Text::JSONNull::Identical(NN<Text::JSONBase> obj)
{
	return obj->GetType() == Text::JSONType::Null;
}

void Text::JSONNull::ToString(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("null"));
}

Text::CStringNN Text::JSONTypeGetName(JSONType t)
{
	switch (t)
	{
	case JSONType::Object:
		return CSTR("Object");
	case JSONType::Array:
		return CSTR("Array");
	case JSONType::Number:
		return CSTR("Number");
	case JSONType::String:
		return CSTR("String");
	case JSONType::BOOL:
		return CSTR("BOOL");
	case JSONType::Null:
		return CSTR("Null");
	case JSONType::INT32:
		return CSTR("INT32");
	case JSONType::INT64:
		return CSTR("INT64");
	default:
		return CSTR("Unknown");
	}
}
