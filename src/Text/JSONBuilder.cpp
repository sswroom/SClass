#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/JSONBuilder.h"
#include "Text/MyStringFloat.h"

void Text::JSONBuilder::AppendStrUTF8(const UTF8Char *val)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while ((c = *val++) != 0)
	{
		if (c == '\"')
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
			*sptr++ = c;
		}
		if (sptr - sbuff >= 254)
		{
			sb->AppendC(sbuff, sptr - sbuff);
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	sb->AppendC(sbuff, sptr - sbuff);
}

void Text::JSONBuilder::AppendStrW(const WChar *val)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF32Char c;
	sptr = sbuff;
	*sptr++ = '\"';
	while ((c = *val++) != 0)
	{
		if (c == '\"')
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
		else if (c < 0x80)
		{
			*sptr++ = (UTF8Char)c;
		}
		else if (c < 0x800)
		{
			*sptr++ = 0xc0 | (c >> 6);
			*sptr++ = 0x80 | (c & 0x3f);
		}
		else if (c >= 0xd800 && c < 0xdc00 && val[0] >= 0xdc00 && val[0] < 0xe000)
		{
			c = 0x10000 + ((c - 0xd800) << 10) + (val[0] - 0xdc00);
			val++;
			if (c < 0x200000)
			{
				*sptr++ = 0xf0 | (c >> 18);
				*sptr++ = 0x80 | ((c >> 12) & 0x3f);
				*sptr++ = 0x80 | ((c >> 6) & 0x3f);
				*sptr++ = 0x80 | (c & 0x3f);
			}
			else if (c < 0x4000000)
			{
				*sptr++ = 0xf8 | (c >> 24);
				*sptr++ = 0x80 | ((c >> 18) & 0x3f);
				*sptr++ = 0x80 | ((c >> 12) & 0x3f);
				*sptr++ = 0x80 | ((c >> 6) & 0x3f);
				*sptr++ = 0x80 | (c & 0x3f);
			}
			else
			{
				*sptr++ = 0xfc | (c >> 30);
				*sptr++ = 0x80 | ((c >> 24) & 0x3f);
				*sptr++ = 0x80 | ((c >> 18) & 0x3f);
				*sptr++ = 0x80 | ((c >> 12) & 0x3f);
				*sptr++ = 0x80 | ((c >> 6) & 0x3f);
				*sptr++ = 0x80 | (c & 0x3f);
			}
		}
		else
		{
			*sptr++ = 0xe0 | (c >> 12);
			*sptr++ = 0x80 | ((c >> 6) & 0x3f);
			*sptr++ = 0x80 | (c & 0x3f);
		}
		if (sptr - sbuff >= 250)
		{
			sb->AppendC(sbuff, sptr - sbuff);
			sptr = sbuff;
		}
	}
	*sptr++ = '\"';
	sb->AppendC(sbuff, sptr - sbuff);
}

Text::JSONBuilder::JSONBuilder(Text::StringBuilderUTF *sb, ObjectType rootType)
{
	NEW_CLASS(this->objTypes, Data::ArrayList<ObjectType>());
	this->sb = sb;
	this->currType = rootType;
	this->isFirst = true;
	if (rootType == OT_ARRAY)
	{
		sb->AppendChar('[', 1);
	}
	else
	{
		sb->AppendChar('{', 1);
	}
}

Text::JSONBuilder::~JSONBuilder()
{
	OSInt i;
	if (this->currType == OT_ARRAY)
	{
		this->sb->AppendChar(']', 1);
	}
	else
	{
		this->sb->AppendChar('}', 1);
	}
	i = this->objTypes->GetCount();
	while (i-- > 0)
	{
		if (this->objTypes->GetItem(i) == OT_OBJECT)
		{
			this->sb->AppendChar('}', 1);
		}
		else
		{
			this->sb->AppendChar(']', 1);
		}
	}
	DEL_CLASS(this->objTypes);
}

Bool Text::JSONBuilder::ArrayAddFloat64(Double val)
{
	if (this->currType != OT_ARRAY)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	Text::SBAppendF64(this->sb, val);
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
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->objTypes->Add(OT_ARRAY);
	this->currType = OT_OBJECT;
	this->isFirst = true;
	this->sb->AppendChar('{', 1);
	return true;
}

Bool Text::JSONBuilder::ArrayEnd()
{
	if (this->currType != OT_ARRAY)
		return false;
	OSInt i = this->objTypes->GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes->RemoveAt(i - 1);
	this->isFirst = false;
	this->sb->AppendChar(']', 1);
	return true;
}

Bool Text::JSONBuilder::ObjectAddFloat64(const UTF8Char *name, Double val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : ", 3);
	Text::SBAppendF64(this->sb, val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt32(const UTF8Char *name, Int32 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : ", 3);
	this->sb->AppendI32(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddInt64(const UTF8Char *name, Int64 val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : ", 3);
	this->sb->AppendI64(val);
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrUTF8(const UTF8Char *name, const UTF8Char *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : ", 3);
	if (val == 0)
	{
		this->sb->AppendC((const UTF8Char*)"null", 4);
	}
	else
	{
		this->AppendStrUTF8(val);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectAddStrW(const UTF8Char *name, const WChar *val)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : ", 3);
	if (val == 0)
	{
		this->sb->AppendC((const UTF8Char*)"null", 4);
	}
	else
	{
		this->AppendStrW(val);
	}
	return true;
}

Bool Text::JSONBuilder::ObjectBeginArray(const UTF8Char *name)
{
	if (this->currType != OT_OBJECT)
		return false;
	if (this->isFirst)
		this->isFirst = false;
	else
	{
		this->sb->AppendC((const UTF8Char*)", ", 2);
	}
	this->AppendStrUTF8(name);
	this->sb->AppendC((const UTF8Char*)" : [", 4);
	this->objTypes->Add(OT_OBJECT);
	this->currType = OT_ARRAY;
	this->isFirst = true;
	return true;
}

Bool Text::JSONBuilder::ObjectEnd()
{
	if (this->currType != OT_OBJECT)
		return false;
	OSInt i = this->objTypes->GetCount();
	if (i <= 0)
		return false;
	this->currType = this->objTypes->RemoveAt(i - 1);
	this->isFirst = false;
	this->sb->AppendChar('}', 1);
	return true;
}
