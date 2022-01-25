#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

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

Text::JSONBase *Text::JSONBase::GetValue(const UTF8Char *path, UOSInt pathLen)
{
	if (Text::StrEqualsC(path, pathLen, UTF8STRC("this")))
	{
		return this;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(path, pathLen);
	UTF8Char *sptr = sb.ToString();
	UTF8Char *sptrEnd = sb.GetEndPtr();
	UOSInt dotIndex;
	UOSInt brkIndex;
	Text::JSONBase *json = this;
	while (json)
	{
		dotIndex = Text::StrIndexOfChar(sptr, '.');
		brkIndex = Text::StrIndexOfChar(sptr, '[');
		if (dotIndex == INVALID_INDEX && brkIndex == INVALID_INDEX)
		{
			if (json->GetType() == JSONType::Object)
			{
				return ((Text::JSONObject*)json)->GetObjectValue(sptr, (UOSInt)(sptrEnd - sptr));
			}
			else if (json->GetType() == JSONType::Array)
			{
				if (Text::StrToUOSInt(sptr, &dotIndex))
				{
					return ((Text::JSONArray*)json)->GetArrayValue(dotIndex);
				}
			}
			return 0;
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
				json = ((Text::JSONObject*)json)->GetObjectValue(sptr, dotIndex);
			}
			else if (json->GetType() == JSONType::Array)
			{
				if (Text::StrToUOSInt(sptr, &brkIndex))
				{
					json = ((Text::JSONArray*)json)->GetArrayValue(brkIndex);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
			sptr += dotIndex + 1;
			if (!isDot)
			{
				if (json == 0)
				{
					return 0;
				}
				dotIndex = Text::StrIndexOfChar(sptr, ']');
				if (dotIndex == INVALID_INDEX)
				{
					return 0;
				}
				if (json->GetType() != JSONType::Array)
				{
					return 0;
				}
				sptr[dotIndex] = 0;
				if (!Text::StrToUOSInt(sptr, &brkIndex))
				{
					return 0;
				}
				json = ((Text::JSONArray*)json)->GetArrayValue(brkIndex);
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
					return 0;
				}
			}
		}
	}
	return 0;
}

Text::String *Text::JSONBase::GetString(const UTF8Char *path, UOSInt pathLen)
{
	Text::JSONBase *json = this->GetValue(path, pathLen);
	if (json && json->IsString())
	{
		return ((Text::JSONString*)json)->GetValue();
	}
	return 0;
}

Text::JSONBase *Text::JSONBase::ParseJSONStr(const UTF8Char *jsonStr)
{
	const UTF8Char *endPtr;
	return ParseJSONStr2(jsonStr, &jsonStr[Text::StrCharCnt(jsonStr)], &endPtr);
}

Text::JSONBase *Text::JSONBase::ParseJSONStrLen(const UTF8Char *jsonStr, UOSInt strLen)
{
	UTF8Char *s = MemAlloc(UTF8Char, strLen + 1);
	const UTF8Char *endPtr;
	Text::JSONBase *ret = ParseJSONStr2(s, Text::StrConcatC(s, jsonStr, strLen), &endPtr);
	MemFree(s);
	return ret;
}

const UTF8Char *Text::JSONBase::ClearWS(const UTF8Char *jsonStr)
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

const UTF8Char *Text::JSONBase::ParseJSString(const UTF8Char *jsonStr, Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UTF8Char c;
	c = *jsonStr++;
	if (c != '\"')
		return 0;
	sptr = sbuff;
	while (true)
	{
		c = *jsonStr++;
		if (c == '\"')
		{
			if (sptr != sbuff)
			{
				*sptr = 0;
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
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
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (UInt32)(c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (UInt32)(c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (UInt32)(c - 0x57);
				else
					return 0;

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
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (UInt32)(c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (UInt32)(c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (UInt32)(c - 0x57);
						else
							return 0;

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
				return 0;
			}
		}
		else if (c == 0)
		{
			return 0;
		}
		else
		{
			*sptr++ = c;
		}
		if ((sptr - sbuff) >= 126)
		{
			*sptr = 0;
			sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	return 0;
}

const UTF8Char *Text::JSONBase::ParseJSNumber(const UTF8Char *jsonStr, Double *val)
{
	UTF8Char sbuff[256];
	UTF8Char *dptr = sbuff;
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
				return 0;
			*dptr++ = c;
			numStart = false;

		}
		else if (c == '.')
		{
			if (hasDot || hasE)
				return 0;
			hasDot = true;
			numStart = false;
			*dptr++ = c;
		}
		else if (c == 'e' || c == 'E')
		{
			if (hasE || numStart)
				return 0;
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
			*val = Text::StrToDouble(sbuff);
			return jsonStr;
		}
		jsonStr++;
	}
}


Text::JSONBase *Text::JSONBase::ParseJSONStr2(const UTF8Char *jsonStr, const UTF8Char *jsonStrEnd, const UTF8Char **jsonStrEndOut)
{
	UTF8Char c;
	jsonStr = ClearWS(jsonStr);
	c = *jsonStr;
	if (c == 0)
	{
		*jsonStrEndOut = 0;
		return 0;
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
			*jsonStrEndOut = jsonStr;
			return jobj;
		}
		while (true)
		{
			jsonStr = ClearWS(jsonStr);
			c = *jsonStr;
			if (c == '\"')
			{
				Text::StringBuilderUTF8 sb;
				Text::JSONBase *obj;
				jsonStr = ParseJSString(jsonStr, &sb);
				if (jsonStr == 0)
				{
					*jsonStrEndOut = 0;
					jobj->EndUse();
					return 0;
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c != ':')
				{
					*jsonStrEndOut = 0;
					jobj->EndUse();
					return 0;
				}
				jsonStr++;
				jsonStr = ClearWS(jsonStr);

				obj = ParseJSONStr2(jsonStr, jsonStrEnd, &jsonStr);
				if (jsonStr == 0)
				{
					jobj->EndUse();
					*jsonStrEndOut = 0;
					return 0;
				}
				jobj->SetObjectValue(sb.ToString(), sb.GetLength(), obj);
				if (obj)
				{
					obj->EndUse();
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c == '}')
				{
					jsonStr++;
					*jsonStrEndOut = jsonStr;
					return jobj;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					jobj->EndUse();
					*jsonStrEndOut = 0;
					return 0;
				}
			}
			else
			{
				*jsonStrEndOut = 0;
				jobj->EndUse();
				return 0;
			}
		}
	}
	else if (c == '[')
	{
		Text::JSONArray *arr;
		Text::JSONBase *obj;

		jsonStr++;
		NEW_CLASS(arr, Text::JSONArray());
		jsonStr = ClearWS(jsonStr);
		if (*jsonStr == ']')
		{
			jsonStr++;
			*jsonStrEndOut = jsonStr;
			return arr;
		}
		while (true)
		{
			c = *jsonStr;
			if (c == 0)
			{
				arr->EndUse();
				*jsonStrEndOut = 0;
				return 0;
			}
			else
			{
				obj = ParseJSONStr2(jsonStr, jsonStrEnd, &jsonStr);
				if (jsonStr == 0)
				{
					arr->EndUse();
					*jsonStrEndOut = 0;
					return 0;
				}
				arr->AddArrayValue(obj);
				if (obj)
				{
					obj->EndUse();
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c == ']')
				{
					jsonStr++;
					*jsonStrEndOut = jsonStr;
					return arr;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					arr->EndUse();
					*jsonStrEndOut = 0;
					return 0;
				}
			}
		}
		return 0;
	}
	else if (c == '\"')
	{
		const UTF8Char *endPtr;
		Text::StringBuilderUTF8 sb;
		endPtr = ParseJSString(jsonStr, &sb);
		if (endPtr == 0)
		{
			*jsonStrEndOut = 0;
			return 0;
		}
		Text::JSONString *s;
		*jsonStrEndOut = endPtr;
		NEW_CLASS(s, Text::JSONString(sb.ToString(), sb.GetLength()));
		return s;
	}
	else if (c == '-' || (c >= '0' && c <= '9'))
	{
		Double val;
		jsonStr = ParseJSNumber(jsonStr, &val);
		if (jsonStr == 0)
		{
			*jsonStrEndOut = 0;
			return 0;
		}
		else 
		{
			Text::JSONNumber *num;
			NEW_CLASS(num, Text::JSONNumber(val));
			*jsonStrEndOut = jsonStr;
			return num;
		}
	}
	else if (c == 't')
	{
		if (Text::StrStartsWithC(jsonStr, (UOSInt)(jsonStrEnd - jsonStr), UTF8STRC("true")))
		{
			Text::JSONBool *b;
			*jsonStrEndOut = &jsonStr[4];
			NEW_CLASS(b, Text::JSONBool(true));
			return b;
		}
		else
		{
			*jsonStrEndOut = 0;
			return 0;
		}
	}
	else if (c == 'f')
	{
		if (Text::StrStartsWithC(jsonStr, (UOSInt)(jsonStrEnd - jsonStr), UTF8STRC("false")))
		{
			Text::JSONBool *b;
			*jsonStrEndOut = &jsonStr[5];
			NEW_CLASS(b, Text::JSONBool(false));
			return b;
		}
		else
		{
			*jsonStrEndOut = 0;
			return 0;
		}
	}
	else if (c == 'n')
	{
		if (Text::StrStartsWithC(jsonStr, (UOSInt)(jsonStrEnd - jsonStr), UTF8STRC("null")))
		{
			Text::JSONNull *n;
			*jsonStrEndOut = &jsonStr[4];
			NEW_CLASS(n, Text::JSONNull());
			return n;
		}
		else
		{
			*jsonStrEndOut = 0;
			return 0;
		}
	}
	else
	{
		*jsonStrEndOut = 0;
		return 0;
	}
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

void Text::JSONNumber::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	Text::SBAppendF64(sb, this->val);
}

Bool Text::JSONNumber::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONNumber::Identical(Text::JSONBase *obj)
{
	if (obj->GetType() != Text::JSONType::Number)
		return false;
	return ((Text::JSONNumber*)obj)->GetValue() == this->val;
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

void Text::JSONInt32::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendI32(this->val);
}

Bool Text::JSONInt32::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONInt32::Identical(Text::JSONBase *obj)
{
	if (obj->GetType() != Text::JSONType::INT32)
		return false;
	return ((Text::JSONInt32*)obj)->GetValue() == this->val;
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

void Text::JSONInt64::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendI64(this->val);
}

Bool Text::JSONInt64::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONInt64::Identical(Text::JSONBase *obj)
{
	if (obj->GetType() != Text::JSONType::INT64)
		return false;
	return ((Text::JSONInt64*)obj)->GetValue() == this->val;
}

Int64 Text::JSONInt64::GetValue()
{
	return this->val;
}

Text::JSONString::JSONString(Text::String *val)
{
	this->val = SCOPY_STRING(val);
}

Text::JSONString::JSONString(const UTF8Char *val, UOSInt leng)
{
	this->val = Text::String::New(val, leng);
}

Text::JSONString::JSONString(const UTF8Char *val)
{
	this->val = Text::String::NewOrNull(val);
}

Text::JSONString::~JSONString()
{
	SDEL_STRING(this->val);
}

Text::JSONType Text::JSONString::GetType()
{
	return Text::JSONType::String;
}

void Text::JSONString::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[128];
	const UTF8Char *sptr;
	UTF8Char *dptr;
	UTF8Char c;
	if (this->val)
	{
		sb->AppendChar('\"', 1);
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
				sb->AppendC(sbuff,(UOSInt)(dptr - sbuff));
				dptr = sbuff;
			}
		}
		if (dptr - sbuff > 0)
		{
			sb->AppendC(sbuff, (UOSInt)(dptr - sbuff));
		}
		sb->AppendChar('\"', 1);
	}
	else
	{
		sb->AppendC((const UTF8Char*)"null", 4);
	}
}

Bool Text::JSONString::Equals(const UTF8Char *s)
{
	if (this->val == 0)
	{
		return s == 0;
	}
	else
	{
		return this->val->Equals(s, Text::StrCharCnt(s));
	}
}

Bool Text::JSONString::Identical(Text::JSONBase *obj)
{
	if (obj->GetType() != Text::JSONType::String)
		return false;
	Text::String *cs = ((Text::JSONString*)obj)->GetValue();
	if (this->val == 0)
	{
		return cs == 0;
	}
	else if (cs == 0)
	{
		return false;
	}
	else
	{
		return cs->Equals(this->val);
	}
}

Text::String *Text::JSONString::GetValue()
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

void Text::JSONBool::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	if (this->val)
	{
		sb->AppendC((const UTF8Char*)"true", 4);
	}
	else
	{
		sb->AppendC((const UTF8Char*)"false", 5);
	}
}

Bool Text::JSONBool::Equals(const UTF8Char *s)
{
	UOSInt len = Text::StrCharCnt(s);
	if  (this->val)
		return Text::StrEqualsICaseC(s, len, UTF8STRC("true"));
	else
		return Text::StrEqualsICaseC(s, len, UTF8STRC("false"));
}

Bool Text::JSONBool::Identical(Text::JSONBase *obj)
{
	if (obj->GetType() != Text::JSONType::BOOL)
		return false;
	return ((Text::JSONBool*)obj)->GetValue() == val;
}

Bool Text::JSONBool::GetValue()
{
	return this->val;
}

Text::JSONObject::JSONObject()
{
	NEW_CLASS(this->objVals, Data::FastStringMap<Text::JSONBase*>());
}

Text::JSONObject::~JSONObject()
{
	UOSInt i;
	Text::JSONBase *obj;
	i = this->objVals->GetCount();
	while (i-- > 0)
	{
		obj = this->objVals->GetItem(i);
		obj->EndUse();
	}
	DEL_CLASS(this->objVals);
}

Text::JSONType Text::JSONObject::GetType()
{
	return Text::JSONType::Object;
}

void Text::JSONObject::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	Text::JSONBase *obj;
	UOSInt i = 0;
	UOSInt j = this->objVals->GetCount();
	sb->AppendC((const UTF8Char*)"{", 1);
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC((const UTF8Char*)", ", 2);
		}
		Text::JSText::ToJSTextDQuote(sb, this->objVals->GetKey(i)->v);
		sb->AppendC((const UTF8Char*)" : ", 3);
		obj = this->objVals->GetItem(i);
		if (obj)
		{
			obj->ToJSONString(sb);
		}
		else
		{
			sb->AppendC((const UTF8Char*)"null", 4);
		}
		i++;
	}
	sb->AppendC((const UTF8Char*)"}", 1);
}

Bool Text::JSONObject::Equals(const UTF8Char *s)
{
	///////////////////////////////
	return false;
}

Bool Text::JSONObject::Identical(Text::JSONBase *obj)
{
	return this == obj;
}

void Text::JSONObject::SetObjectValue(const UTF8Char *name, UOSInt nameLen, Text::JSONBase *val)
{
	if (val)
	{
		val->BeginUse();
	}
	Text::JSONBase *obj = this->objVals->GetC(name, nameLen);
	if (obj)
	{
		obj->EndUse();
	}
	this->objVals->Put(name, val);
}

Text::JSONBase *Text::JSONObject::GetObjectValue(const UTF8Char *name, UOSInt nameLen)
{
	return this->objVals->GetC(name, nameLen);
}

void Text::JSONObject::GetObjectNames(Data::ArrayList<Text::String *> *names)
{
	UOSInt i = 0;
	UOSInt j = this->objVals->GetCount();
	while (i < j)
	{
		names->Add(this->objVals->GetKey(i));
		i++;
	}
}

Text::String *Text::JSONObject::GetObjectString(const UTF8Char *name, UOSInt nameLen)
{
	Text::JSONBase *baseObj = this->objVals->GetC(name, nameLen);
	if (baseObj == 0 || baseObj->GetType() != Text::JSONType::String)
	{
		return 0;
	}
	return ((Text::JSONString*)baseObj)->GetValue();
}

Text::JSONArray::JSONArray()
{
	NEW_CLASS(this->arrVals, Data::ArrayList<Text::JSONBase*>());
}

Text::JSONArray::~JSONArray()
{
	UOSInt i = this->arrVals->GetCount();
	while (i-- > 0)
	{
		this->arrVals->GetItem(i)->EndUse();
	}
	DEL_CLASS(this->arrVals);
}

Text::JSONType Text::JSONArray::GetType()
{
	return Text::JSONType::Array;
}

void Text::JSONArray::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	Text::JSONBase *obj;
	UOSInt i = 0;
	UOSInt j = this->arrVals->GetCount();
	sb->AppendC((const UTF8Char*)"[", 1);
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC((const UTF8Char*)", ", 2);
		}
		obj = this->arrVals->GetItem(i);
		if (obj)
		{
			obj->ToJSONString(sb);
		}
		else
		{
			sb->AppendC((const UTF8Char*)"null", 4);
		}
		i++;
	}
	sb->AppendC((const UTF8Char*)"]", 1);
}

Bool Text::JSONArray::Equals(const UTF8Char *s)
{
	///////////////////////////////
	return false;
}

Bool Text::JSONArray::Identical(Text::JSONBase *obj)
{
	return this == obj;
}

void Text::JSONArray::SetArrayValue(UOSInt index, Text::JSONBase *val)
{
	if (val)
	{
		val->BeginUse();
	}
	Text::JSONBase *obj;
	obj = this->arrVals->GetItem(index);
	if (obj)
	{
		obj->EndUse();
	}
	this->arrVals->SetItem(index, val);
	////////////////////////////////////
}

void Text::JSONArray::AddArrayValue(Text::JSONBase *val)
{
	if (val)
	{
		val->BeginUse();
	}
	this->arrVals->Add(val);
}

Text::JSONBase *Text::JSONArray::GetArrayValue(UOSInt index)
{
	return this->arrVals->GetItem(index);
}

UOSInt Text::JSONArray::GetArrayLength()
{
	return this->arrVals->GetCount();
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

void Text::JSONNull::ToJSONString(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC((const UTF8Char*)"null", 4);
}

Bool Text::JSONNull::Equals(const UTF8Char *s)
{
	return Text::StrEquals(s, (const UTF8Char*)"null");
}

Bool Text::JSONNull::Identical(Text::JSONBase *obj)
{
	return obj->GetType() == Text::JSONType::Null;
}

const UTF8Char *Text::JSONTypeGetName(JSONType t)
{
	switch (t)
	{
	case JSONType::Object:
		return (const UTF8Char*)"Object";
	case JSONType::Array:
		return (const UTF8Char*)"Array";
	case JSONType::Number:
		return (const UTF8Char*)"Number";
	case JSONType::String:
		return (const UTF8Char*)"String";
	case JSONType::BOOL:
		return (const UTF8Char*)"BOOL";
	case JSONType::Null:
		return (const UTF8Char*)"Null";
	case JSONType::INT32:
		return (const UTF8Char*)"INT32";
	case JSONType::INT64:
		return (const UTF8Char*)"INT64";
	case JSONType::StringWO:
		return (const UTF8Char*)"StringWO";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
