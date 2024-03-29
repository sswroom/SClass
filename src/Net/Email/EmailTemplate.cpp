#include "Stdafx.h"
#include "Net/Email/EmailTemplate.h"
#include "Text/XML.h"

Bool Net::Email::EmailTemplate::ParseTemplate(const UTF8Char *tpl, NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringUTF8Map<const UTF8Char *> *vars)
{
	UOSInt i;
	const UTF8Char *paramName;
	const UTF8Char *param;
	Text::String *s;
	while (true)
	{
		i = Text::StrIndexOfChar(tpl, '[');
		if (i == INVALID_INDEX)
		{
			sb->AppendSlow(tpl);
			return true;
		}
		if (i > 0)
		{
			sb->AppendC(tpl, i);
		}
		tpl += i;
		if (tpl[1] == '[')
		{
			sb->AppendUTF8Char('[');
			tpl += 2;
		}
		else
		{
			i = Text::StrIndexOfChar(tpl, ']');
			if (i == INVALID_INDEX)
			{
				return false;
			}
			if (tpl[1] == '@' || tpl[1] == '#')
			{
				paramName = Text::StrCopyNewC(tpl + 2, i - 2);
			}
			else
			{
				paramName = Text::StrCopyNewC(tpl + 1, i - 1);
			}
			param = vars->Get(paramName);
			if (param == 0)
			{
				Text::StrDelNew(paramName);
				return false;
			}
			if (tpl[1] == '@')
			{
				s = Text::XML::ToNewAttrText(param);
				sb->Append(s);
				s->Release();
			}
			else if (tpl[1] == '#')
			{
				s = Text::XML::ToNewHTMLText(param);
				sb->Append(s);
				s->Release();
			}
			else
			{
				sb->AppendSlow(param);
			}
			tpl += i + 1;
		}
	}
}

Net::Email::EmailTemplate::EmailTemplate(const UTF8Char *tpl, Data::StringUTF8Map<const UTF8Char *> *vars)
{
	this->itemTemplate = 0;
	this->itemOfst = 0;
	this->sbSubj = 0;
	this->sbPre = 0;
	this->sbItem = 0;
	this->sbPost = 0;
	this->error = false;

	Text::StringBuilderUTF8 sb;
	NEW_CLASS(this->sbSubj, Text::StringBuilderUTF8());
	Text::PString sarr[2];
	sb.AppendSlow(tpl);
	UOSInt i = Text::StrSplitLineP(sarr, 2, sb);
	if (i == 1)
	{
		this->error = true;
		return;
	}
	if (!this->ParseTemplate(sarr[0].v, this->sbSubj, vars))
	{
		this->error = true;
		return;
	}
	this->itemOfst = Text::StrIndexOfC(sarr[1].v, sarr[1].leng, UTF8STRC("[item]"));
	if (this->itemOfst == INVALID_INDEX)
	{
		NEW_CLASS(this->sbPre, Text::StringBuilderUTF8());
		if (!this->ParseTemplate(sarr[1].v, this->sbPre, vars))
		{
			this->error = true;
		}
	}
	else
	{
		i = Text::StrIndexOfC(sarr[1].v, sarr[1].leng, UTF8STRC("[/item]"));
		if (i == INVALID_INDEX || i < this->itemOfst)
		{
			this->error = true;
			return;
		}
		this->itemTemplate = Text::StrCopyNewC(sarr[1].v + this->itemOfst + 6, i - this->itemOfst - 6);
		NEW_CLASS(this->sbPre, Text::StringBuilderUTF8());
		NEW_CLASS(this->sbPost, Text::StringBuilderUTF8());
		NEW_CLASS(this->sbItem, Text::StringBuilderUTF8());
		
		sarr[1].v[this->itemOfst] = 0;
		sarr[1].leng = this->itemOfst;
		if (!this->ParseTemplate(sarr[1].v, this->sbPre, vars))
		{
			this->error = true;
		}
		if (!this->ParseTemplate(sarr[1].v + i + 7, this->sbPost, vars))
		{
			this->error = true;
		}
	}
}

Net::Email::EmailTemplate::~EmailTemplate()
{
	SDEL_TEXT(this->itemTemplate);
	SDEL_CLASS(this->sbItem);
	SDEL_CLASS(this->sbPre);
	SDEL_CLASS(this->sbPost);
	SDEL_CLASS(this->sbSubj);
}

Bool Net::Email::EmailTemplate::IsError()
{
	return this->error;
}

Bool Net::Email::EmailTemplate::AddItem(Data::StringUTF8Map<const UTF8Char *> *itemVars)
{
	if (this->itemTemplate == 0)
	{
		return false;
	}
	return this->ParseTemplate(this->itemTemplate, this->sbItem, itemVars);
}

const UTF8Char *Net::Email::EmailTemplate::GetSubject()
{
	return this->sbSubj->ToString();
}

void Net::Email::EmailTemplate::GetContent(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->itemTemplate == 0)
	{
		sb->Append(this->sbPre);
	}
	else
	{
		sb->Append(this->sbPre);
		sb->Append(this->sbItem);
		sb->Append(this->sbPost);
	}
}
