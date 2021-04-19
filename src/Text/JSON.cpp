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

Bool Text::JSONBase::IsStringUTF8()
{
	return this->GetJSType() == JST_STRINGUTF8;
}

Text::JSONBase *Text::JSONBase::ParseJSONStr(const UTF8Char *jsonStr)
{
	const UTF8Char *endPtr;
	return ParseJSONStr2(jsonStr, &endPtr);
}

Text::JSONBase *Text::JSONBase::ParseJSONStrLen(const UTF8Char *jsonStr, UOSInt strLen)
{
	UTF8Char *s = MemAlloc(UTF8Char, strLen + 1);
	Text::StrConcatC(s, jsonStr, strLen);
	const UTF8Char *endPtr;
	Text::JSONBase *ret = ParseJSONStr2(s, &endPtr);
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

const UTF8Char *Text::JSONBase::ParseJSString(const UTF8Char *jsonStr, Text::StringBuilderUTF *sb)
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
				sb->AppendC(sbuff, sptr - sbuff);
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
					v = (v << 4) + (c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (c - 0x57);
				else
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (c - 0x57);
				else
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (c - 0x57);
				else
					return 0;
				c = *jsonStr++;
				if (c >= '0' && c <= '9')
					v = (v << 4) + (c - '0');
				else if (c >= 'A' && c <= 'F')
					v = (v << 4) + (c - 0x37);
				else if (c >= 'a' && c <= 'f')
					v = (v << 4) + (c - 0x57);
				else
					return 0;

				if (v < 0x80)
				{
					*sptr++ = (UInt8)v;
				}
				else if (v < 0x800)
				{
					*sptr++ = 0xc0 | (v >> 6);
					*sptr++ = 0x80 | (v & 0x3f);
				}
				else if (v >= 0xd800 && v < 0xdc00)
				{
					if (jsonStr[0] == '\\' && jsonStr[1] == 'u')
					{
						UInt32 v2 = 0;
						jsonStr += 2;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (c - 0x57);
						else
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (c - 0x57);
						else
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (c - 0x57);
						else
							return 0;
						c = *jsonStr++;
						if (c >= '0' && c <= '9')
							v2 = (v2 << 4) + (c - '0');
						else if (c >= 'A' && c <= 'F')
							v2 = (v2 << 4) + (c - 0x37);
						else if (c >= 'a' && c <= 'f')
							v2 = (v2 << 4) + (c - 0x57);
						else
							return 0;

						if (v2 >= 0xdc00 && v2 < 0xe000)
						{
							v = 0x10000 + ((v - 0xd800) << 10) + (v2 - 0xdc00);
							if (v < 0x200000)
							{
								*sptr++ = 0xf0 | (v >> 18);
								*sptr++ = 0x80 | ((v >> 12) & 0x3f);
								*sptr++ = 0x80 | ((v >> 6) & 0x3f);
								*sptr++ = 0x80 | (v & 0x3f);
							}
							else if (v < 0x4000000)
							{
								*sptr++ = 0xf8 | (v >> 24);
								*sptr++ = 0x80 | ((v >> 18) & 0x3f);
								*sptr++ = 0x80 | ((v >> 12) & 0x3f);
								*sptr++ = 0x80 | ((v >> 6) & 0x3f);
								*sptr++ = 0x80 | (v & 0x3f);
							}
							else
							{
								*sptr++ = 0xfc | (v >> 30);
								*sptr++ = 0x80 | ((v >> 24) & 0x3f);
								*sptr++ = 0x80 | ((v >> 18) & 0x3f);
								*sptr++ = 0x80 | ((v >> 12) & 0x3f);
								*sptr++ = 0x80 | ((v >> 6) & 0x3f);
								*sptr++ = 0x80 | (v & 0x3f);
							}
						}
						else
						{
							*sptr++ = 0xe0 | (v >> 12);
							*sptr++ = 0x80 | ((v >> 6) & 0x3f);
							*sptr++ = 0x80 | (v & 0x3f);
							*sptr++ = 0xe0 | (v2 >> 12);
							*sptr++ = 0x80 | ((v2 >> 6) & 0x3f);
							*sptr++ = 0x80 | (v2 & 0x3f);
						}
					}
					else
					{
						*sptr++ = 0xe0 | (v >> 12);
						*sptr++ = 0x80 | ((v >> 6) & 0x3f);
						*sptr++ = 0x80 | (v & 0x3f);
					}
					
				}
				else
				{
					*sptr++ = 0xe0 | (v >> 12);
					*sptr++ = 0x80 | ((v >> 6) & 0x3f);
					*sptr++ = 0x80 | (v & 0x3f);
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
			sb->AppendC(sbuff, sptr - sbuff);
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


Text::JSONBase *Text::JSONBase::ParseJSONStr2(const UTF8Char *jsonStr, const UTF8Char **jsonStrEnd)
{
	UTF8Char c;
	jsonStr = ClearWS(jsonStr);
	c = *jsonStr;
	if (c == 0)
	{
		*jsonStrEnd = 0;
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
			*jsonStrEnd = jsonStr;
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
					*jsonStrEnd = 0;
					jobj->EndUse();
					return 0;
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c != ':')
				{
					*jsonStrEnd = 0;
					jobj->EndUse();
					return 0;
				}
				jsonStr++;
				jsonStr = ClearWS(jsonStr);

				obj = ParseJSONStr2(jsonStr, &jsonStr);
				if (jsonStr == 0)
				{
					jobj->EndUse();
					*jsonStrEnd = 0;
					return 0;
				}
				jobj->SetObjectValue(sb.ToString(), obj);
				if (obj)
				{
					obj->EndUse();
				}
				jsonStr = ClearWS(jsonStr);
				c = *jsonStr;
				if (c == '}')
				{
					jsonStr++;
					*jsonStrEnd = jsonStr;
					return jobj;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					jobj->EndUse();
					*jsonStrEnd = 0;
					return 0;
				}
			}
			else
			{
				*jsonStrEnd = 0;
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
			*jsonStrEnd = jsonStr;
			return arr;
		}
		while (true)
		{
			c = *jsonStr;
			if (c == 0)
			{
				arr->EndUse();
				*jsonStrEnd = 0;
				return 0;
			}
			else
			{
				obj = ParseJSONStr2(jsonStr, &jsonStr);
				if (jsonStr == 0)
				{
					arr->EndUse();
					*jsonStrEnd = 0;
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
					*jsonStrEnd = jsonStr;
					return arr;
				}
				else if (c == ',')
				{
					jsonStr++;
				}
				else
				{
					arr->EndUse();
					*jsonStrEnd = 0;
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
			*jsonStrEnd = 0;
			return 0;
		}
		Text::JSONStringUTF8 *s;
		*jsonStrEnd = endPtr;
		NEW_CLASS(s, Text::JSONStringUTF8(sb.ToString()));
		return s;
	}
	else if (c == '-' || (c >= '0' && c <= '9'))
	{
		Double val;
		jsonStr = ParseJSNumber(jsonStr, &val);
		if (jsonStr == 0)
		{
			*jsonStrEnd = 0;
			return 0;
		}
		else 
		{
			Text::JSONNumber *num;
			NEW_CLASS(num, Text::JSONNumber(val));
			*jsonStrEnd = jsonStr;
			return num;
		}
	}
	else if (c == 't')
	{
		if (Text::StrStartsWith(jsonStr, (const UTF8Char*)"true"))
		{
			Text::JSONBool *b;
			*jsonStrEnd = &jsonStr[4];
			NEW_CLASS(b, Text::JSONBool(true));
			return b;
		}
		else
		{
			*jsonStrEnd = 0;
			return 0;
		}
	}
	else if (c == 'f')
	{
		if (Text::StrStartsWith(jsonStr, (const UTF8Char*)"false"))
		{
			Text::JSONBool *b;
			*jsonStrEnd = &jsonStr[5];
			NEW_CLASS(b, Text::JSONBool(false));
			return b;
		}
		else
		{
			*jsonStrEnd = 0;
			return 0;
		}
	}
	else if (c == 'n')
	{
		if (Text::StrStartsWith(jsonStr, (const UTF8Char*)"null"))
		{
			Text::JSONNull *n;
			*jsonStrEnd = &jsonStr[4];
			NEW_CLASS(n, Text::JSONNull());
			return n;
		}
		else
		{
			*jsonStrEnd = 0;
			return 0;
		}
	}
	else
	{
		*jsonStrEnd = 0;
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

Text::JSONBase::JSType Text::JSONNumber::GetJSType()
{
	return Text::JSONBase::JST_NUMBER;
}

void Text::JSONNumber::ToJSONString(Text::StringBuilderUTF *sb)
{
	Text::SBAppendF64(sb, this->val);
}

Bool Text::JSONNumber::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONNumber::Identical(Text::JSONBase *obj)
{
	if (obj->GetJSType() != Text::JSONBase::JST_NUMBER)
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

Text::JSONBase::JSType Text::JSONInt32::GetJSType()
{
	return Text::JSONBase::JST_INT32;
}

void Text::JSONInt32::ToJSONString(Text::StringBuilderUTF *sb)
{
	sb->AppendI32(this->val);
}

Bool Text::JSONInt32::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONInt32::Identical(Text::JSONBase *obj)
{
	if (obj->GetJSType() != Text::JSONBase::JST_INT32)
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

Text::JSONBase::JSType Text::JSONInt64::GetJSType()
{
	return Text::JSONBase::JST_INT64;
}

void Text::JSONInt64::ToJSONString(Text::StringBuilderUTF *sb)
{
	sb->AppendI64(this->val);
}

Bool Text::JSONInt64::Equals(const UTF8Char *s)
{
	return false;
}

Bool Text::JSONInt64::Identical(Text::JSONBase *obj)
{
	if (obj->GetJSType() != Text::JSONBase::JST_INT64)
		return false;
	return ((Text::JSONInt64*)obj)->GetValue() == this->val;
}

Int64 Text::JSONInt64::GetValue()
{
	return this->val;
}

Text::JSONStringUTF8::JSONStringUTF8(const UTF8Char *val)
{
	if (val)
	{
		this->val = Text::StrCopyNew(val);
	}
	else
	{
		this->val = 0;
	}
}

Text::JSONStringUTF8::~JSONStringUTF8()
{
	SDEL_TEXT(this->val);
}

Text::JSONBase::JSType Text::JSONStringUTF8::GetJSType()
{
	return Text::JSONBase::JST_STRINGUTF8;
}

void Text::JSONStringUTF8::ToJSONString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[128];
	const UTF8Char *sptr;
	UTF8Char *dptr;
	UTF8Char c;
	if (this->val)
	{
		sb->AppendChar('\"', 1);
		sptr = this->val;
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
				sb->AppendC(sbuff, dptr - sbuff);
				dptr = sbuff;
			}
		}
		if (dptr - sbuff > 0)
		{
			sb->AppendC(sbuff, dptr - sbuff);
		}
		sb->AppendChar('\"', 1);
	}
	else
	{
		sb->AppendC((const UTF8Char*)"null", 4);
	}
}

Bool Text::JSONStringUTF8::Equals(const UTF8Char *s)
{
	if (this->val == 0)
	{
		return s == 0;
	}
	else
	{
		return Text::StrEquals(this->val, s);
	}
}

Bool Text::JSONStringUTF8::Identical(Text::JSONBase *obj)
{
	if (obj->GetJSType() != Text::JSONBase::JST_STRINGUTF8)
		return false;
	const UTF8Char *cs = ((Text::JSONStringUTF8*)obj)->GetValue();
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
		return Text::StrEquals(cs, this->val);
	}
}

const UTF8Char *Text::JSONStringUTF8::GetValue()
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

Text::JSONBase::JSType Text::JSONBool::GetJSType()
{
	return Text::JSONBase::JST_BOOL;
}

void Text::JSONBool::ToJSONString(Text::StringBuilderUTF *sb)
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
	if  (this->val)
		return Text::StrEqualsICase(s, (const UTF8Char*)"true");
	else
		return Text::StrEqualsICase(s, (const UTF8Char*)"false");
}

Bool Text::JSONBool::Identical(Text::JSONBase *obj)
{
	if (obj->GetJSType() != Text::JSONBase::JST_BOOL)
		return false;
	return ((Text::JSONBool*)obj)->GetValue() == val;
}

Bool Text::JSONBool::GetValue()
{
	return this->val;
}

Text::JSONObject::JSONObject()
{
	NEW_CLASS(this->objVals, Data::StringUTF8Map<Text::JSONBase*>());
}

Text::JSONObject::~JSONObject()
{
	OSInt i;
	Text::JSONBase *obj;
	Data::ArrayList<Text::JSONBase*> *vals = this->objVals->GetValues();
	i = vals->GetCount();
	while (i-- > 0)
	{
		obj = vals->GetItem(i);
		obj->EndUse();
	}
	DEL_CLASS(this->objVals);
}

Text::JSONBase::JSType Text::JSONObject::GetJSType()
{
	return Text::JSONBase::JST_OBJECT;
}

void Text::JSONObject::ToJSONString(Text::StringBuilderUTF *sb)
{
	Data::ArrayList<const UTF8Char *> *keys = this->objVals->GetKeys();
	Data::ArrayList<Text::JSONBase*> *vals = this->objVals->GetValues();
	Text::JSONBase *obj;
	OSInt i = 0;
	OSInt j = keys->GetCount();
	sb->AppendC((const UTF8Char*)"{", 1);
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC((const UTF8Char*)", ", 2);
		}
		Text::JSText::ToJSTextDQuote(sb, keys->GetItem(i));
		sb->AppendC((const UTF8Char*)" : ", 3);
		obj = vals->GetItem(i);
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

void Text::JSONObject::SetObjectValue(const UTF8Char *name, Text::JSONBase *val)
{
	if (val)
	{
		val->BeginUse();
	}
	Text::JSONBase *obj = this->objVals->Get(name);
	if (obj)
	{
		obj->EndUse();
	}
	this->objVals->Put(name, val);
}

Text::JSONBase *Text::JSONObject::GetObjectValue(const UTF8Char *name)
{
	return this->objVals->Get(name);
}

void Text::JSONObject::GetObjectNames(Data::ArrayList<const UTF8Char *> *names)
{
	names->AddRange(this->objVals->GetKeys());
}

Text::JSONArray::JSONArray()
{
	NEW_CLASS(this->arrVals, Data::ArrayList<Text::JSONBase*>());
}

Text::JSONArray::~JSONArray()
{
	OSInt i = this->arrVals->GetCount();
	while (i-- > 0)
	{
		this->arrVals->GetItem(i)->EndUse();
	}
	DEL_CLASS(this->arrVals);
}

Text::JSONBase::JSType Text::JSONArray::GetJSType()
{
	return Text::JSONBase::JST_ARRAY;
}

void Text::JSONArray::ToJSONString(Text::StringBuilderUTF *sb)
{
	Text::JSONBase *obj;
	OSInt i = 0;
	OSInt j = this->arrVals->GetCount();
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

Text::JSONBase::JSType Text::JSONNull::GetJSType()
{
	return Text::JSONBase::JST_NULL;
}

void Text::JSONNull::ToJSONString(Text::StringBuilderUTF *sb)
{
	sb->AppendC((const UTF8Char*)"null", 4);
}

Bool Text::JSONNull::Equals(const UTF8Char *s)
{
	return Text::StrEquals(s, (const UTF8Char*)"null");
}

Bool Text::JSONNull::Identical(Text::JSONBase *obj)
{
	return obj->GetJSType() == Text::JSONBase::JST_NULL;
}
