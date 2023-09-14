#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JSONBuilder.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

void Text::JSONBuilder::AppendStr(Text::CStringNN val)
{
	this->AppendStrUTF8(val.v);
}

void Text::JSONBuilder::AppendStrUTF8(const UTF8Char *val)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while ((c = *val++) != 0)
	{
		switch (c)
		{
		case '\\':
			sptr[0] = '\\';
			sptr[1] = '\\';
			sptr += 2;
			break;
		case '\"':
			sptr[0] = '\\';
			sptr[1] = '\"';
			sptr += 2;
			break;
		case '\r':
			sptr[0] = '\\';
			sptr[1] = 'r';
			sptr += 2;
			break;
		case '\n':
			sptr[0] = '\\';
			sptr[1] = 'n';
			sptr += 2;
			break;
		default:
			*sptr++ = c;
			break;
		}
		if (sptr - sbuff >= 254)
		{
			this->sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	this->sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
}

void Text::JSONBuilder::AppendStrW(const WChar *val)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF32Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while (true)
	{
		val = Text::StrReadChar(val, &c);
		if (c == 0)
		{
			break;
		}
		else if (c == '\"')
		{
			*sptr++ = '\\';
			*sptr++ = '\\';
		}
		else if (c == '\"')
		{
			*sptr++ = '\\';
			*sptr++ = '\"';
		}
		else if (c == '\r')
		{
			*sptr++ = '\\';
			*sptr++ = 'r';
		}
		else if (c == '\n')
		{
			*sptr++ = '\\';
			*sptr++ = 'n';
		}
		else
		{
			sptr = Text::StrWriteChar(sptr, c);
		}
		if (sptr - sbuff >= 250)
		{
			this->sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	this->sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
}

Text::JSONBuilder::JSONBuilder(NotNullPtr<Text::StringBuilderUTF8> sb, ObjectType rootType)
{
	this->sb = sb;
	this->currType = rootType;
	this->isFirst = true;
	if (rootType == OT_ARRAY)
	{
		sb->AppendUTF8Char('[');
	}
	else
	{
		sb->AppendUTF8Char('{');
	}
}

Text::JSONBuilder::~JSONBuilder()
{
	UOSInt i;
	if (this->currType == OT_ARRAY)
	{
		this->sb->AppendUTF8Char(']');
	}
	else
	{
		this->sb->AppendUTF8Char('}');
	}
	i = this->objTypes.GetCount();
	while (i-- > 0)
	{
		if (this->objTypes.GetItem(i) == OT_OBJECT)
		{
			this->sb->AppendUTF8Char('}');
		}
		else
		{
			this->sb->AppendUTF8Char(']');
		}
	}
}

Bool Text::JSONBuilder::ArrayAddInt32(Int32 val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->sb->AppendI32(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddInt64(Int64 val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->sb->AppendI64(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddFloat64(Double val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->sb->AppendDouble(val);
	return true;
}

Bool Text::JSONBuilder::ArrayAddBool(Bool val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->sb->Append(val?CSTR("true"):CSTR("false"));
	return true;
}

Bool Text::JSONBuilder::ArrayAddStr(Text::PString *val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	if (val == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(val->v);
	}
	return true;
}

Bool Text::JSONBuilder::ArrayAddStrUTF8(const UTF8Char *val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	if (val == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(val);
	}
	return true;
}

Bool Text::JSONBuilder::ArrayAddNull()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->sb->AppendC(UTF8STRC("null"));
	return true;
}

Bool Text::JSONBuilder::ArrayAdd(NotNullPtr<Text::JSONArray> arr)
{
	if (this->currType != OT_ARRAY)
		return false;
	UOSInt i = 0;
	UOSInt j = arr->GetArrayLength();
	NotNullPtr<Text::JSONBase> json;
	while (i < j)
	{
		if (json.Set(arr->GetArrayValue(i)))
		{
			switch (json->GetType())
			{
			case Text::JSONType::Object:
				this->ArrayBeginObject();
				this->ObjectAdd(NotNullPtr<Text::JSONObject>::ConvertFrom(json));
				this->ObjectEnd();
				break;
			case Text::JSONType::Array:
				this->ArrayBeginArray();
				this->ArrayAdd(NotNullPtr<Text::JSONArray>::ConvertFrom(json));
				this->ArrayEnd();
				break;
			case Text::JSONType::Number:
				this->ArrayAddFloat64(((Text::JSONNumber*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::String:
				this->ArrayAddStr(((Text::JSONString*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::BOOL:
				this->ArrayAddBool(((Text::JSONBool*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::Null:
				this->ArrayAddNull();
				break;
			case Text::JSONType::INT32:
				this->ArrayAddInt32(((Text::JSONInt32*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::INT64:
				this->ArrayAddInt64(((Text::JSONInt64*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::StringWO:
				break;
			}
		}
		else
		{
			this->ArrayAddNull();
		}
		i++;
	}
	return true;
}

Bool Text::JSONBuilder::ArrayBeginObject()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->objTypes.Add(OT_ARRAY);
	this->currType = OT_OBJECT;
	this->isFirst = true;
	this->sb->AppendUTF8Char('{');
	return true;
}

Bool Text::JSONBuilder::ArrayBeginArray()
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->objTypes.Add(OT_ARRAY);
	this->currType = OT_ARRAY;
	this->isFirst = true;
	this->sb->AppendUTF8Char('[');
	return true;
}

Bool Text::JSONBuilder::ArrayEnd()
{
	if (this->currType != OT_ARRAY)
		return false;
	UOSInt i = this->objTypes.GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes.RemoveAt(i - 1);
	this->isFirst = false;
	this->sb->AppendUTF8Char(']');
	return true;
}

Bool Text::JSONBuilder::ObjectAddFloat64(Text::CStringNN name, Double val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->sb->AppendDouble(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt32(Text::CStringNN name, Int32 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->sb->AppendI32(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt64(Text::CStringNN name, Int64 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->sb->AppendI64(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddUInt64(Text::CStringNN name, UInt64 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->sb->AppendU64(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddBool(Text::CStringNN name, Bool val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->sb->Append(val?CSTR("true"):CSTR("false"));
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, Text::PString *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	if (val == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStr(val->ToCString());
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, NotNullPtr<Text::String> val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	this->AppendStr(val->ToCString());
	return true;
}

Bool Text::JSONBuilder::ObjectAddStr(Text::CStringNN name, Text::CString val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	if (val.v == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStr(val.OrEmpty());
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrUTF8(Text::CStringNN name, const UTF8Char *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	if (val == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrUTF8(val);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrW(Text::CStringNN name, const WChar *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":"));
	if (val == 0)
	{
		this->sb->AppendC(UTF8STRC("null"));
	}
	else
	{
		this->AppendStrW(val);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddNull(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":null"));
	return true;
}

Bool Text::JSONBuilder::ObjectAdd(NotNullPtr<Text::JSONObject> obj)
{
	if (this->currType != OT_OBJECT)
		return false;
	Data::ArrayList<Text::String*> names;
	obj->GetObjectNames(names);
	UOSInt i = 0;
	UOSInt j = names.GetCount();
	NotNullPtr<Text::JSONBase> json;
	Text::String *name;
	while (i < j)
	{
		name = names.GetItem(i);
		if (json.Set(obj->GetObjectValue(name->ToCString())))
		{
			switch (json->GetType())
			{
			case Text::JSONType::Object:
				this->ObjectBeginObject(name->ToCString());
				this->ObjectAdd(NotNullPtr<Text::JSONObject>::ConvertFrom(json));
				this->ObjectEnd();
				break;
			case Text::JSONType::Array:
				this->ObjectBeginArray(name->ToCString());
				this->ArrayAdd(NotNullPtr<Text::JSONArray>::ConvertFrom(json));
				this->ArrayEnd();
				break;
			case Text::JSONType::Number:
				this->ObjectAddFloat64(name->ToCString(), ((Text::JSONNumber*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::String:
				this->ObjectAddStr(name->ToCString(), ((Text::JSONString*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::BOOL:
				this->ObjectAddBool(name->ToCString(), ((Text::JSONBool*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::Null:
				this->ObjectAddNull(name->ToCString());
				break;
			case Text::JSONType::INT32:
				this->ObjectAddInt32(name->ToCString(), ((Text::JSONInt32*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::INT64:
				this->ObjectAddInt64(name->ToCString(), ((Text::JSONInt64*)json.Ptr())->GetValue());
				break;
			case Text::JSONType::StringWO:
				break;
			}
		}
		i++;
	}
	return true;
}

Bool Text::JSONBuilder::ObjectBeginArray(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":["));
	this->objTypes.Add(OT_OBJECT);
	this->currType = OT_ARRAY;
	this->isFirst = true;
	return true;
}

Bool Text::JSONBuilder::ObjectBeginObject(Text::CStringNN name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC(UTF8STRC(","));
	}
	this->AppendStr(name);
	this->sb->AppendC(UTF8STRC(":{"));
	this->objTypes.Add(OT_OBJECT);
	this->currType = OT_OBJECT;
	this->isFirst = true;
	return true;
}

Bool Text::JSONBuilder::ObjectEnd()
{
	if (this->currType != OT_OBJECT)
		return false;
	UOSInt i = this->objTypes.GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes.RemoveAt(i - 1);
	this->isFirst = false;
	this->sb->AppendUTF8Char('}');
	return true;
}
