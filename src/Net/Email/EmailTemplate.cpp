#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/Email/EmailTemplate.h"
#include "Text/UTF8Reader.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/FormEncoding.h"

Bool Net::Email::EmailTemplate::ParseTemplate(NN<Text::StringBuilderUTF8> sbOut, Text::CStringNN tpl, NN<Data::FastStringMapNN<Text::String>> items, NN<IO::LogTool> log)
{
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sbParam;
	Text::StringBuilderUTF8 sb;
	Text::CStringNN paramName;
	NN<Text::String> param;
	i = 0;
	while (true)
	{
		j = tpl.IndexOf('[', i);
		if (j == INVALID_INDEX)
		{
			sbOut->Append(tpl.Substring(i));
			return true;
		}
		if (i != j)
		{
			sbOut->AppendC(tpl.v + i, j - i);
		}
		i = j + 1;
		if (tpl.v[i] == '[')
		{
			sbOut->AppendUTF8Char('[');
			i++;
		}
		else
		{
			j = tpl.IndexOf(']', i);
			if (j == INVALID_INDEX)
			{
				log->LogMessage(CSTR("EmailTemplate ']' not found after '['"), IO::LogHandler::LogLevel::Error);
				return false;
			}
			sbParam.ClearStr();
			sbParam.AppendC(tpl.v + i, j - i);
			paramName = sbParam.ToCString();
			while (paramName.v[0] == '@' || paramName.v[0] == '#' || paramName.v[0] == '^' || paramName.v[0] == '$')
			{
				paramName = paramName.Substring(1);
			}
			if (!items->GetC(paramName).SetTo(param))
			{
				sbParam.ClearStr();
				sbParam.Append(CSTR("EmailTemplate item ["));
				sbParam.AppendC(tpl.v + i, j - i);
				sbParam.Append(CSTR("] not found"));
				log->LogMessage(sbParam.ToCString(), IO::LogHandler::LogLevel::Error);
				return false;
			}
			sbParam.ClearStr();
			sbParam.Append(param);
			while (true)
			{
				if (tpl.v[i] == '@')
				{
					param = Text::XML::ToNewXMLText(sbParam.v);
					sbParam.ClearStr();
					sbParam.Append(param);
					param->Release();
				}
				else if (tpl.v[i] == '#')
				{
					param = Text::XML::ToNewHTMLBodyText(sbParam.v);
					sbParam.ClearStr();
					sbParam.Append(param);
					param->Release();
				}
				else if (tpl.v[i] == '^')
				{
					sb.ClearStr();
					Text::TextBinEnc::FormEncoding::FormEncode(sb, sbParam.ToCString());
					sbParam.ClearStr();
					sbParam.Append(sb);
				}
				else if (tpl.v[i] == '$')
				{
				}
				else
				{
					sbOut->Append(sbParam);
					break;
				}
				i++;
			}
			i = j + 1;
		}
	}
}

Net::Email::EmailTemplate::EmailTemplate(Text::CStringNN subject, Text::CStringNN content, Bool htmlContent)
{
	this->subject = Text::String::New(subject);
	this->content = Text::String::New(content);
	this->htmlContent = htmlContent;
}

Net::Email::EmailTemplate::~EmailTemplate()
{
	this->subject->Release();
	this->content->Release();
}

Bool Net::Email::EmailTemplate::FillEmailMessage(NN<Net::Email::EmailMessage> msg, NN<Data::FastStringMapNN<Text::String>> items, NN<IO::LogTool> log)
{
	Text::StringBuilderUTF8 sb;
	if (!ParseTemplate(sb, this->subject->ToCString(), items, log))
		return false;
	msg->SetSubject(sb.ToCString());
	sb.ClearStr();		
	if (!ParseTemplate(sb, this->content->ToCString(), items, log))
		return false;
	msg->SetContent(sb.ToCString(), this->htmlContent?CSTR("text/html"):CSTR("text/plain"));
	return true;
}

Optional<Net::Email::EmailTemplate> Net::Email::EmailTemplate::LoadFromFile(Text::CStringNN fileName, Bool htmlContent)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return 0;
	Text::StringBuilderUTF8 sbSubject;
	Text::StringBuilderUTF8 sbContent;
	Text::UTF8Reader reader(fs);
	while (true)
	{
		if (!reader.ReadLine(sbSubject, 2048))
			return 0;
		if (reader.IsLineBreak())
			break;
	}
	reader.ReadToEnd(sbContent);
	if (sbSubject.leng > 0 && sbContent.leng > 0)
	{
		NN<Net::Email::EmailTemplate> tpl;
		NEW_CLASSNN(tpl, Net::Email::EmailTemplate(sbSubject.ToCString(), sbContent.ToCString(), htmlContent));
		return tpl;
	}
	return 0;
}
