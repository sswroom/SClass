#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/MemoryStream.h"
#include "IO/StreamReader.h"
#include "Net/SDPFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void Net::SDPFile::InitSDP()
{
	this->buff = 0;
	this->buffSize = 0;

	this->version = 0;
	this->sessName = 0;
	this->userName = 0;
	this->sessId = 0;
	this->sessVer = 0;
	this->userAddrType = 0;
	this->userAddrHost = 0;
	this->startTime = -1;
	this->endTime = -1;
	this->sessTool = 0;
	this->sessType = 0;
	this->sessSend = false;
	this->sessRecv = true;
	this->sessCharset = 0;
	this->sessControl = 0;
	this->reqUserAgent = 0;
	NEW_CLASS(this->sessDesc, Data::ArrayListStrUTF8());
	NEW_CLASS(this->sessDescType, Data::ArrayListInt32());
	NEW_CLASS(this->sessMedia, Data::ArrayList<Net::ISDPMedia*>());
	NEW_CLASS(this->mediaList, Data::ArrayList<Data::ArrayList<const UTF8Char *>*>());

}

Net::SDPFile::SDPFile(UInt8 *buff, UOSInt buffSize)
{
	InitSDP();
	this->buff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->buff, buff, buffSize);

	UTF8Char sbuff[256];
	UTF8Char *sarr[7];
	Data::ArrayListStrUTF8 *currMedia = 0;
	IO::MemoryStream *mstm;
	Text::UTF8Reader *reader;
	NEW_CLASS(mstm, IO::MemoryStream(this->buff, this->buffSize, UTF8STRC("Net.SDPFile.SDPFile")));
	NEW_CLASS(reader, Text::UTF8Reader(mstm));
	while (reader->ReadLine(sbuff, 255))
	{
		if (sbuff[1] == '=')
		{
			switch (sbuff[0])
			{
			case 'v': //protocol version
				this->version = Text::StrToInt32(&sbuff[2]);
				break;
			case 'o': //owner/creator and session identifier
				if (Text::StrSplit(sarr, 7, &sbuff[2], ' ') == 6)
				{
					SDEL_TEXT(this->userName);
					SDEL_TEXT(this->sessId);
					SDEL_TEXT(this->sessVer);
					SDEL_TEXT(this->userAddrType);
					SDEL_TEXT(this->userAddrHost);
					this->userName = Text::StrCopyNew(sarr[0]);
					this->sessId = Text::StrCopyNew(sarr[1]);
					this->sessVer = Text::StrCopyNew(sarr[2]);
					this->userAddrType = Text::StrCopyNew(sarr[4]);
					this->userAddrHost = Text::StrCopyNew(sarr[5]);
				}
				break;
			case 's': //session name
				SDEL_TEXT(this->sessName);
				this->sessName = Text::StrCopyNew(&sbuff[2]);
				break;
			case 'i': //session information
				this->sessDesc->Add(Text::StrCopyNew(&sbuff[2]));
				this->sessDescType->Add('i');
				break;
			case 'u': //URI of description
				this->sessDesc->Add(Text::StrCopyNew(&sbuff[2]));
				this->sessDescType->Add('u');
				break;
			case 'e': //email address
				this->sessDesc->Add(Text::StrCopyNew(&sbuff[2]));
				this->sessDescType->Add('e');
				break;
			case 'p': //phone number
				this->sessDesc->Add(Text::StrCopyNew(&sbuff[2]));
				this->sessDescType->Add('p');
				break;
			case 'c': //connection information - not required if included in all media
				break;
			case 'b': //bandwidth information
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNew(sbuff));
				}
				break;
			case 'z': //time zone adjustments
				break;
			case 'k': //encryption key
				break;
			case 'a': //zero or more session attribute lines
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNew(sbuff));
				}
				else
				{
					if (Text::StrStartsWith(sbuff, (const UTF8Char*)"a=tool:"))
					{
						SDEL_TEXT(this->sessTool);
						this->sessTool = Text::StrCopyNew(&sbuff[7]);
					}
					else if (Text::StrCompare(sbuff, (const UTF8Char*)"a=recvonly") == 0)
					{
						this->sessSend = false;
						this->sessRecv = true;
					}
					else if (Text::StrCompare(sbuff, (const UTF8Char*)"a=sendonly") == 0)
					{
						this->sessSend = true;
						this->sessRecv = false;
					}
					else if (Text::StrCompare(sbuff, (const UTF8Char*)"a=sendrecv") == 0)
					{
						this->sessSend = true;
						this->sessRecv = true;
					}
					else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"a=type:"))
					{
						SDEL_TEXT(this->sessType);
						this->sessType = Text::StrCopyNew(&sbuff[7]);
					}
					else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"a=charset:"))
					{
						SDEL_TEXT(this->sessCharset);
						this->sessCharset = Text::StrCopyNew(&sbuff[10]);
					}
					else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"a=control:"))
					{
						SDEL_TEXT(this->sessControl);
						this->sessControl = Text::StrCopyNew(&sbuff[10]);
					}
				}
				break;
			case 't': //time the session is active
				if (Text::StrSplit(sarr, 3, &sbuff[2], ' ') == 2)
				{
					this->startTime = Text::StrToInt64(sarr[0]);
					this->endTime = Text::StrToInt64(sarr[1]);
				}
				break;
			case 'r': //zero or more repeat times
				break;
			case 'm': //media name and transport address
				NEW_CLASS(currMedia, Data::ArrayListStrUTF8());
				currMedia->Add(Text::StrCopyNew(sbuff));
				this->mediaList->Add(currMedia);
				break;
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(mstm);
}

Net::SDPFile::SDPFile()
{
	InitSDP();
}

Net::SDPFile::~SDPFile()
{
	UOSInt i;
	UOSInt j;
	Data::ArrayList<const UTF8Char *> *currMedia;
	MemFree(this->buff);
	SDEL_TEXT(this->sessName);
	SDEL_TEXT(this->userName);
	SDEL_TEXT(this->sessId);
	SDEL_TEXT(this->sessVer);
	SDEL_TEXT(this->userAddrType);
	SDEL_TEXT(this->userAddrHost);
	SDEL_TEXT(this->sessTool);
	SDEL_TEXT(this->sessType);
	SDEL_TEXT(this->sessCharset);
	SDEL_TEXT(this->sessControl);
	SDEL_TEXT(this->reqUserAgent);

	i = this->sessDesc->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->sessDesc->RemoveAt(i));
	}
	DEL_CLASS(this->sessDesc);
	DEL_CLASS(this->sessDescType);
	i = this->mediaList->GetCount();
	while (i-- > 0)
	{
		currMedia = this->mediaList->RemoveAt(i);
		j = currMedia->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(currMedia->RemoveAt(j));
		}
		DEL_CLASS(currMedia);
	}
	DEL_CLASS(this->mediaList);
	DEL_CLASS(this->sessMedia);
}

void Net::SDPFile::SetDefaults()
{
	UTF8Char sbuff[32];
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StrInt64(sbuff, dt.ToDotNetTicks());

	this->SetUserName((const UTF8Char*)"-");
	this->SetSessId(sbuff);
	this->SetSessVer(sbuff);
	this->SetUserAddrType((const UTF8Char*)"IP4");
	this->SetUserAddrHost((const UTF8Char*)"127.0.0.1");
	this->SetSessName((const UTF8Char*)"Unnamed");
	this->SetStartTime(0);
	this->SetEndTime(0);
	this->SetSessTool((const UTF8Char*)"RTSP");
	this->SetSessCharset((const UTF8Char*)"UTF-8");
}

void Net::SDPFile::SetVersion(Int32 version)
{
	this->version = version;
}

void Net::SDPFile::SetUserName(const UTF8Char *userName)
{
	SDEL_TEXT(this->userName);
	this->userName = Text::StrCopyNew(userName);
}

void Net::SDPFile::SetSessName(const UTF8Char *sessName)
{
	SDEL_TEXT(this->sessName);
	this->sessName = Text::StrCopyNew(sessName);
}

void Net::SDPFile::SetSessId(const UTF8Char *sessId)
{
	SDEL_TEXT(this->sessId);
	this->sessId = Text::StrCopyNew(sessId);
}

void Net::SDPFile::SetSessVer(const UTF8Char *sessVer)
{
	SDEL_TEXT(this->sessVer);
	this->sessVer = Text::StrCopyNew(sessVer);
}

void Net::SDPFile::SetUserAddrType(const UTF8Char *userAddrType)
{
	SDEL_TEXT(this->userAddrType);
	this->userAddrType = Text::StrCopyNew(userAddrType);
}

void Net::SDPFile::SetUserAddrHost(const UTF8Char *userAddrHost)
{
	SDEL_TEXT(this->userAddrHost);
	this->userAddrHost = Text::StrCopyNew(userAddrHost);
}

void Net::SDPFile::SetStartTime(Int64 startTime)
{
	this->startTime = startTime;
}

void Net::SDPFile::SetEndTime(Int64 endTime)
{
	this->endTime = endTime;
}

void Net::SDPFile::SetSessTool(const UTF8Char *sessTool)
{
	SDEL_TEXT(this->sessTool);
	this->sessTool = Text::StrCopyNew(sessTool);
}

void Net::SDPFile::SetSessType(const UTF8Char *sessType)
{
	SDEL_TEXT(this->sessType);
	this->sessType = Text::StrCopyNew(sessType);
}

void Net::SDPFile::SetSessCharset(const UTF8Char *sessCharset)
{
	SDEL_TEXT(this->sessCharset);
	this->sessCharset = Text::StrCopyNew(sessCharset);
}

void Net::SDPFile::SetSessControl(const UTF8Char *sessControl)
{
	SDEL_TEXT(this->sessControl);
	this->sessControl = Text::StrCopyNew(sessControl);
}

void Net::SDPFile::SetReqUserAgent(const UTF8Char *userAgent)
{
	SDEL_TEXT(this->reqUserAgent);
	this->reqUserAgent = Text::StrCopyNew(userAgent);
}

void Net::SDPFile::AddBuildMedia(Net::ISDPMedia *media)
{
	this->sessMedia->Add(media);
}

Bool Net::SDPFile::BuildBuff()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UTF8Char sbuff[3];
	IO::MemoryStream *mstm;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Net.SDPFile.BuildBuff")));
	NEW_CLASS(writer, Text::UTF8Writer(mstm));
	sb.ClearStr();

	sb.AppendC(UTF8STRC("v="));
	sb.AppendI32(this->version);
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("o="));
	sb.Append(this->userName);
	sb.AppendC(UTF8STRC(" "));
	sb.Append(this->sessId);
	sb.AppendC(UTF8STRC(" "));
	sb.Append(this->sessVer);
	sb.AppendC(UTF8STRC(" IN "));
	sb.Append(this->userAddrType);
	sb.AppendC(UTF8STRC(" "));
	sb.Append(this->userAddrHost);
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("s="));
	sb.Append(this->sessName);
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	i = 0;
	j = this->sessDesc->GetCount();
	while (i < j)
	{
		sb.ClearStr();
		sbuff[0] = (UTF8Char)this->sessDescType->GetItem(i);
		sbuff[1] = '=';
		sbuff[2] = 0;
		sb.Append(sbuff);
		sb.Append(this->sessDesc->GetItem(i));
		i++;
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("c=IN IP4 0.0.0.0"));
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("t="));
	sb.AppendI64(this->startTime);
	sb.AppendC(UTF8STRC(" "));
	sb.AppendI64(this->endTime);
	writer->WriteLineC(sb.ToString(), sb.GetLength());

	if (this->sessTool)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=tool:"));
		sb.Append(this->sessTool);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	if (this->sessSend && !this->sessRecv)
	{
		writer->WriteLineC(UTF8STRC("a=sendonly"));
	}
	else if (this->sessSend && this->sessRecv)
	{
		writer->WriteLineC(UTF8STRC("a=sendrecv"));
	}
	else
	{
		writer->WriteLineC(UTF8STRC("a=recvonly"));
	}
	if (this->sessCharset)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=charset:"));
		sb.Append(this->sessCharset);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}
	if (this->sessControl)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=control:"));
		sb.Append(this->sessControl);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
	}

	i = 0;
	j = this->sessMedia->GetCount();
	while (i < j)
	{
		Net::ISDPMedia *media = this->sessMedia->GetItem(i);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("m="));
		if (media->GetSDPMediaType() == Media::MEDIA_TYPE_VIDEO)
		{
			sb.AppendC(UTF8STRC("video"));
		}
		else if (media->GetSDPMediaType() == Media::MEDIA_TYPE_AUDIO)
		{
			sb.AppendC(UTF8STRC("audio"));
		}
		sb.AppendC(UTF8STRC(" "));
		sb.AppendU16(media->GetSDPMediaPort());
		sb.AppendC(UTF8STRC(" "));
		sb.Append(media->GetSDPProtocol());
		k = 0;
		l = media->GetSDPDataCount();
		while (k < l)
		{
			sb.AppendC(UTF8STRC(" "));
			sb.AppendU16(media->GetSDPData(k)->GetSDPDataPort());

			k++;
		}
		writer->WriteLineC(sb.ToString(), sb.GetLength());

		k = 0;
		l = media->GetSDPDataCount();
		while (k < l)
		{
			Net::ISDPData *data = media->GetSDPData(k);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=rtpmap:"));
			sb.AppendU16(data->GetSDPDataPort());
			sb.AppendC(UTF8STRC(" "));
			sb.AllocLeng(512);
			sb.SetEndPtr(data->GetSDPDataType(sb.GetEndPtr()));
			sb.AppendC(UTF8STRC("/"));
			sb.AppendU32(data->GetSDPDataFreq());
			writer->WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=fmtp:"));
			sb.AppendU16(data->GetSDPDataPort());
			sb.AppendC(UTF8STRC(" "));
			sb.AllocLeng(512);
			sb.SetEndPtr(data->GetSDPDataFormat(sb.GetEndPtr()));
			writer->WriteLineC(sb.ToString(), sb.GetLength());
			k++;
		}

		if (media->GetSDPControlURL(this->reqUserAgent))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=control:"));
			sb.Append(media->GetSDPControlURL(this->reqUserAgent));
			writer->WriteLineC(sb.ToString(), sb.GetLength());
		}

		i++;
	}

	DEL_CLASS(writer);
	UOSInt buffSize;
	UInt8 *buff;
	buff = mstm->GetBuff(&buffSize);
	if (this->buff)
	{
		MemFree(this->buff);
	}
	this->buff = MemAlloc(UInt8, buffSize);
	MemCopyNO(this->buff, buff, buffSize);
	this->buffSize = buffSize;
	DEL_CLASS(mstm);
	return true;
}

Bool Net::SDPFile::WriteToStream(IO::Stream *stm)
{
	return stm->Write(this->buff, this->buffSize) == this->buffSize;
}

UOSInt Net::SDPFile::GetLength()
{
	return this->buffSize;
}

UOSInt Net::SDPFile::GetMediaCount()
{
	return this->mediaList->GetCount();
}

Data::ArrayList<const UTF8Char *> *Net::SDPFile::GetMediaDesc(UOSInt index)
{
	return this->mediaList->GetItem(index);
}
