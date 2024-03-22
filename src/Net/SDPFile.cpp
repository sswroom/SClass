#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/MemoryReadingStream.h"
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
}

Net::SDPFile::SDPFile(UInt8 *buff, UOSInt buffSize)
{
	InitSDP();
	this->buff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->buff, buff, buffSize);

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString sarr[7];
	Data::ArrayListStrUTF8 *currMedia = 0;
	IO::MemoryReadingStream mstm(this->buff, this->buffSize);
	Text::UTF8Reader reader(mstm);
	while ((sptr = reader.ReadLine(sbuff, 255)) != 0)
	{
		if (sbuff[1] == '=')
		{
			switch (sbuff[0])
			{
			case 'v': //protocol version
				this->version = Text::StrToInt32(&sbuff[2]);
				break;
			case 'o': //owner/creator and session identifier
				if (Text::StrSplitP(sarr, 7, {&sbuff[2], (UOSInt)(sptr - &sbuff[2])}, ' ') == 6)
				{
					SDEL_STRING(this->userName);
					SDEL_STRING(this->sessId);
					SDEL_STRING(this->sessVer);
					SDEL_STRING(this->userAddrType);
					SDEL_STRING(this->userAddrHost);
					this->userName = Text::String::New(sarr[0].ToCString()).Ptr();
					this->sessId = Text::String::New(sarr[1].ToCString()).Ptr();
					this->sessVer = Text::String::New(sarr[2].ToCString()).Ptr();
					this->userAddrType = Text::String::New(sarr[4].ToCString()).Ptr();
					this->userAddrHost = Text::String::New(sarr[5].ToCString()).Ptr();
				}
				break;
			case 's': //session name
				SDEL_STRING(this->sessName);
				this->sessName = Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])).Ptr();
				break;
			case 'i': //session information
				this->sessDesc.Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType.Add('i');
				break;
			case 'u': //URI of description
				this->sessDesc.Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType.Add('u');
				break;
			case 'e': //email address
				this->sessDesc.Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType.Add('e');
				break;
			case 'p': //phone number
				this->sessDesc.Add(Text::String::New(&sbuff[2], (UOSInt)(sptr - &sbuff[2])));
				this->sessDescType.Add('p');
				break;
			case 'c': //connection information - not required if included in all media
				break;
			case 'b': //bandwidth information
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)).Ptr());
				}
				break;
			case 'z': //time zone adjustments
				break;
			case 'k': //encryption key
				break;
			case 'a': //zero or more session attribute lines
				if (currMedia)
				{
					currMedia->Add(Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)).Ptr());
				}
				else
				{
					if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=tool:")))
					{
						SDEL_STRING(this->sessTool);
						this->sessTool = Text::String::NewP(&sbuff[7], sptr).Ptr();
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
						this->sessType = Text::String::NewP(&sbuff[7], sptr).Ptr();
					}
					else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=charset:")))
					{
						SDEL_STRING(this->sessCharset);
						this->sessCharset = Text::String::NewP(&sbuff[10], sptr).Ptr();
					}
					else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("a=control:")))
					{
						SDEL_STRING(this->sessControl);
						this->sessControl = Text::String::NewP(&sbuff[10], sptr).Ptr();
					}
				}
				break;
			case 't': //time the session is active
				if (Text::StrSplitP(sarr, 3, {&sbuff[2], (UOSInt)(sptr - &sbuff[2])}, ' ') == 2)
				{
					this->startTime = Text::StrToInt64(sarr[0].v);
					this->endTime = Text::StrToInt64(sarr[1].v);
				}
				break;
			case 'r': //zero or more repeat times
				break;
			case 'm': //media name and transport address
				NEW_CLASS(currMedia, Data::ArrayListStrUTF8());
				currMedia->Add(Text::StrCopyNew(sbuff).Ptr());
				this->mediaList.Add(currMedia);
				break;
			}
		}
	}
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

	i = this->sessDesc.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->sessDesc.RemoveAt(i));
	}
	i = this->mediaList.GetCount();
	while (i-- > 0)
	{
		currMedia = this->mediaList.RemoveAt(i);
		j = currMedia->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(currMedia->RemoveAt(j));
		}
		DEL_CLASS(currMedia);
	}
}

void Net::SDPFile::SetDefaults()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(sbuff, dt.ToDotNetTicks());

	this->SetUserName(CSTR("-"));
	this->SetSessId(CSTRP(sbuff, sptr));
	this->SetSessVer(CSTRP(sbuff, sptr));
	this->SetUserAddrType(CSTR("IP4"));
	this->SetUserAddrHost(CSTR("127.0.0.1"));
	this->SetSessName(CSTR("Unnamed"));
	this->SetStartTime(0);
	this->SetEndTime(0);
	this->SetSessTool(CSTR("RTSP"));
	this->SetSessCharset(CSTR("UTF-8"));
}

void Net::SDPFile::SetVersion(Int32 version)
{
	this->version = version;
}

void Net::SDPFile::SetUserName(Text::CString userName)
{
	SDEL_STRING(this->userName);
	this->userName = Text::String::New(userName).Ptr();
}

void Net::SDPFile::SetSessName(Text::CString sessName)
{
	SDEL_STRING(this->sessName);
	this->sessName = Text::String::New(sessName).Ptr();
}

void Net::SDPFile::SetSessId(Text::CString sessId)
{
	SDEL_STRING(this->sessId);
	this->sessId = Text::String::New(sessId).Ptr();
}

void Net::SDPFile::SetSessVer(Text::CString sessVer)
{
	SDEL_STRING(this->sessVer);
	this->sessVer = Text::String::New(sessVer).Ptr();
}

void Net::SDPFile::SetUserAddrType(Text::CString userAddrType)
{
	SDEL_STRING(this->userAddrType);
	this->userAddrType = Text::String::New(userAddrType).Ptr();
}

void Net::SDPFile::SetUserAddrHost(Text::CString userAddrHost)
{
	SDEL_STRING(this->userAddrHost);
	this->userAddrHost = Text::String::New(userAddrHost).Ptr();
}

void Net::SDPFile::SetStartTime(Int64 startTime)
{
	this->startTime = startTime;
}

void Net::SDPFile::SetEndTime(Int64 endTime)
{
	this->endTime = endTime;
}

void Net::SDPFile::SetSessTool(Text::CString sessTool)
{
	SDEL_STRING(this->sessTool);
	this->sessTool = Text::String::New(sessTool).Ptr();
}

void Net::SDPFile::SetSessType(Text::CString sessType)
{
	SDEL_STRING(this->sessType);
	this->sessType = Text::String::New(sessType).Ptr();
}

void Net::SDPFile::SetSessCharset(Text::CString sessCharset)
{
	SDEL_STRING(this->sessCharset);
	this->sessCharset = Text::String::New(sessCharset).Ptr();
}

void Net::SDPFile::SetSessControl(Text::CString sessControl)
{
	SDEL_STRING(this->sessControl);
	this->sessControl = Text::String::New(sessControl).Ptr();
}

void Net::SDPFile::SetReqUserAgent(Text::CString userAgent)
{
	SDEL_STRING(this->reqUserAgent);
	this->reqUserAgent = Text::String::New(userAgent).Ptr();
}

void Net::SDPFile::AddBuildMedia(Net::ISDPMedia *media)
{
	this->sessMedia.Add(media);
}

Bool Net::SDPFile::BuildBuff()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UTF8Char sbuff[3];
	Text::StringBuilderUTF8 sb;
	IO::MemoryStream mstm;
	Text::UTF8Writer writer(mstm);
	sb.ClearStr();

	sb.AppendC(UTF8STRC("v="));
	sb.AppendI32(this->version);
	writer.WriteLineC(sb.ToString(), sb.GetLength());

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
	writer.WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("s="));
	sb.Append(this->sessName);
	writer.WriteLineC(sb.ToString(), sb.GetLength());

	Data::ArrayIterator<NotNullPtr<Text::String>> it = this->sessDesc.Iterator();
	i = 0;
	while (it.HasNext())
	{
		sb.ClearStr();
		sbuff[0] = (UTF8Char)this->sessDescType.GetItem(i);
		sbuff[1] = '=';
		sbuff[2] = 0;
		sb.AppendC(sbuff, 2);
		sb.Append(it.Next());
		i++;
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("c=IN IP4 0.0.0.0"));
	writer.WriteLineC(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("t="));
	sb.AppendI64(this->startTime);
	sb.AppendC(UTF8STRC(" "));
	sb.AppendI64(this->endTime);
	writer.WriteLineC(sb.ToString(), sb.GetLength());

	if (this->sessTool)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=tool:"));
		sb.Append(this->sessTool);
		writer.WriteLineC(sb.ToString(), sb.GetLength());
	}
	if (this->sessSend && !this->sessRecv)
	{
		writer.WriteLineC(UTF8STRC("a=sendonly"));
	}
	else if (this->sessSend && this->sessRecv)
	{
		writer.WriteLineC(UTF8STRC("a=sendrecv"));
	}
	else
	{
		writer.WriteLineC(UTF8STRC("a=recvonly"));
	}
	if (this->sessCharset)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=charset:"));
		sb.Append(this->sessCharset);
		writer.WriteLineC(sb.ToString(), sb.GetLength());
	}
	if (this->sessControl)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("a=control:"));
		sb.Append(this->sessControl);
		writer.WriteLineC(sb.ToString(), sb.GetLength());
	}

	i = 0;
	j = this->sessMedia.GetCount();
	while (i < j)
	{
		Net::ISDPMedia *media = this->sessMedia.GetItem(i);
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
		writer.WriteLineC(sb.ToString(), sb.GetLength());

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
			writer.WriteLineC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=fmtp:"));
			sb.AppendU16(data->GetSDPDataPort());
			sb.AppendC(UTF8STRC(" "));
			sb.AllocLeng(512);
			sb.SetEndPtr(data->GetSDPDataFormat(sb.GetEndPtr()));
			writer.WriteLineC(sb.ToString(), sb.GetLength());
			k++;
		}

		Text::CString ctrlURL = media->GetSDPControlURL(this->reqUserAgent->ToCString());
		if (ctrlURL.leng > 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("a=control:"));
			sb.Append(ctrlURL);
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}

		i++;
	}
	UOSInt buffSize;
	UInt8 *buff;
	buff = mstm.GetBuff(buffSize);
	if (this->buff)
	{
		MemFree(this->buff);
	}
	this->buff = MemAlloc(UInt8, buffSize);
	MemCopyNO(this->buff, buff, buffSize);
	this->buffSize = buffSize;
	return true;
}

Bool Net::SDPFile::WriteToStream(NotNullPtr<IO::Stream> stm)
{
	return stm->Write(this->buff, this->buffSize) == this->buffSize;
}

UOSInt Net::SDPFile::GetLength()
{
	return this->buffSize;
}

UOSInt Net::SDPFile::GetMediaCount()
{
	return this->mediaList.GetCount();
}

Data::ArrayList<const UTF8Char *> *Net::SDPFile::GetMediaDesc(UOSInt index)
{
	return this->mediaList.GetItem(index);
}
