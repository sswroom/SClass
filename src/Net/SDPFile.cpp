#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListString.h"
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
	NEW_CLASS(this->sessDesc, Data::ArrayListString());
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
	UTF8Char *sptr;
	UTF8Char *sarr[7];
	Data::ArrayListStrUTF8 *currMedia = 0;
	IO::MemoryStream *mstm;
	Text::UTF8Reader *reader;
	NEW_CLASS(mstm, IO::MemoryStream(this->buff, this->buffSize, UTF8STRC("Net.SDPFile.SDPFile")));
	NEW_CLASS(reader, Text::UTF8Reader(mstm));
	while ((sptr = reader->ReadLine(sbuff, 255)) != 0)
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
					SDEL_STRING(this->userName);
					SDEL_STRING(this->sessId);
					SDEL_STRING(this->sessVer);
					SDEL_STRING(this->userAddrType);
					SDEL_STRING(this->userAddrHost);
					this->userName = Text::String::NewNotNull(sarr[0]);
					this->sessId = Text::String::NewNotNull(sarr[1]);
					this->sessVer = Text::String::NewNotNull(sarr[2]);
					this->userAddrType = Text::String::NewNotNull(sarr[4]);
					this->userAddrHost = Text::String::NewNotNull(sarr[5]);
				}
				break;
			case 's': //session name
				SDEL_STRING(this->sessName);
				this->sessName = Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2]));
				break;
			case 'i': //session information
				this->sessDesc->Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType->Add('i');
				break;
			case 'u': //URI of description
				this->sessDesc->Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType->Add('u');
				break;
			case 'e': //email address
				this->sessDesc->Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType->Add('e');
				break;
			case 'p': //phone number
				this->sessDesc->Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType->Add('p');
				break;
			case 'c': //connection information - not required if included in all media
				break;
			case 'b': //bandwidth information
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)));
				}
				break;
			case 'z': //time zone adjustments
				break;
			case 'k': //encryption key
				break;
			case 'a': //zero or more session attribute lines
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=tool:")))
					{
						SDEL_STRING(this->sessTool);
						this->sessTool = Text::String::NewNotNull(&sbuff[7]);
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=recvonly")))
					{
						this->sessSend = false;
						this->sessRecv = true;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=sendonly")))
					{
						this->sessSend = true;
						this->sessRecv = false;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=sendrecv")))
					{
						this->sessSend = true;
						this->sessRecv = true;
					}
					else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=type:")))
					{
						SDEL_STRING(this->sessType);
						this->sessType = Text::String::NewNotNull(&sbuff[7]);
					}
					else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=charset:")))
					{
						SDEL_STRING(this->sessCharset);
						this->sessCharset = Text::String::NewNotNull(&sbuff[10]);
					}
					else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=control:")))
					{
						SDEL_STRING(this->sessControl);
						this->sessControl = Text::String::NewNotNull(&sbuff[10]);
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
	SDEL_STRING(this->sessName);
	SDEL_STRING(this->userName);
	SDEL_STRING(this->sessId);
	SDEL_STRING(this->sessVer);
	SDEL_STRING(this->userAddrType);
	SDEL_STRING(this->userAddrHost);
	SDEL_STRING(this->sessTool);
	SDEL_STRING(this->sessType);
	SDEL_STRING(this->sessCharset);
	SDEL_STRING(this->sessControl);
	SDEL_STRING(this->reqUserAgent);

	i = this->sessDesc->GetCount();
	while (i-- > 0)
	{
		this->sessDesc->RemoveAt(i)->Release();
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
	SDEL_STRING(this->userName);
	this->userName = Text::String::NewNotNull(userName);
}

void Net::SDPFile::SetSessName(const UTF8Char *sessName)
{
	SDEL_STRING(this->sessName);
	this->sessName = Text::String::NewNotNull(sessName);
}

void Net::SDPFile::SetSessId(const UTF8Char *sessId)
{
	SDEL_STRING(this->sessId);
	this->sessId = Text::String::NewNotNull(sessId);
}

void Net::SDPFile::SetSessVer(const UTF8Char *sessVer)
{
	SDEL_STRING(this->sessVer);
	this->sessVer = Text::String::NewNotNull(sessVer);
}

void Net::SDPFile::SetUserAddrType(const UTF8Char *userAddrType)
{
	SDEL_STRING(this->userAddrType);
	this->userAddrType = Text::String::NewNotNull(userAddrType);
}

void Net::SDPFile::SetUserAddrHost(const UTF8Char *userAddrHost)
{
	SDEL_STRING(this->userAddrHost);
	this->userAddrHost = Text::String::NewNotNull(userAddrHost);
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
	SDEL_STRING(this->sessTool);
	this->sessTool = Text::String::NewNotNull(sessTool);
}

void Net::SDPFile::SetSessType(const UTF8Char *sessType)
{
	SDEL_STRING(this->sessType);
	this->sessType = Text::String::NewNotNull(sessType);
}

void Net::SDPFile::SetSessCharset(const UTF8Char *sessCharset)
{
	SDEL_STRING(this->sessCharset);
	this->sessCharset = Text::String::NewNotNull(sessCharset);
}

void Net::SDPFile::SetSessControl(const UTF8Char *sessControl)
{
	SDEL_STRING(this->sessControl);
	this->sessControl = Text::String::NewNotNull(sessControl);
}

void Net::SDPFile::SetReqUserAgent(const UTF8Char *userAgent)
{
	SDEL_STRING(this->reqUserAgent);
	this->reqUserAgent = Text::String::NewNotNull(userAgent);
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
		sb.AppendC(sbuff, 2);
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

		Text::CString ctrlURL = media->GetSDPControlURL(this->reqUserAgent->ToCString());
		if (ctrlURL.leng > 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=control:"));
			sb.Append(ctrlURL);
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
