#include "Stdafx.h"
#include "IO/Device/OlympusCameraControl.h"
#include "Net/ConnectionInfo.h"
#include "Net/HTTPClient.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/XMLReader.h"

void IO::Device::OlympusCameraControl::GetCommandList()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	Text::StringBuilderUTF8 sb;
	Text::XMLAttrib *attr;
	UOSInt i;
	OSInt j;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/get_commandlist.cgi"));
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);
	NN<Text::String> nodeName;
	while (reader.NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("oishare")))
		{
			while (reader.NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("version")))
				{
					sb.ClearStr();
					reader.ReadNodeText(sb);
					sb.TrimWSCRLF();
					SDEL_STRING(this->oiVersion);
					this->oiVersion = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
				else if (nodeName->Equals(UTF8STRC("oitrackversion")))
				{
					sb.ClearStr();
					reader.ReadNodeText(sb);
					sb.TrimWSCRLF();
					SDEL_STRING(this->oiTrackVersion);
					this->oiTrackVersion = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
				else if (nodeName->Equals(UTF8STRC("support")))
				{
					reader.SkipElement();
				}
				else if (nodeName->Equals(UTF8STRC("cgi")))
				{
					i = reader.GetAttribCount();
					while (i-- > 0)
					{
						attr = reader.GetAttrib(i);
						if (attr->value && attr->name->Equals(UTF8STRC("name")))
						{
							j = this->cmdList.SortedIndexOf(Text::String::OrEmpty(attr->value));
							if (j < 0)
							{
								this->cmdList.SortedInsert(attr->value->Clone());
							}
						}
					}
					reader.SkipElement();
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
		else
		{
			reader.SkipElement();
		}
	}
	cli.Delete();
}

void IO::Device::OlympusCameraControl::GetImageList()
{
	if (this->fileList == 0)
	{
		NEW_CLASS(this->fileList, Data::ArrayListNN<IO::Device::OlympusCameraControl::FileInfo>());
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		NN<Net::HTTPClient> cli;
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 sb;
		NN<IO::CameraControl::FileInfo> file;
		UTF8Char *sarr[7];
		Data::DateTime dt;
		Int32 dateVal;
		Int32 timeVal;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
		sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
		sptr = Text::StrConcatC(sptr, UTF8STRC("/get_imglist.cgi?DIR=/DCIM/100OLYMP"));
		cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		NEW_CLASS(reader, Text::UTF8Reader(cli));
		while (true)
		{
			sb.ClearStr();
			if (!reader->ReadLine(sb, 1024))
			{
				break;
			}
			if (Text::StrSplit(sarr, 7, sb.v, ',') == 6)
			{
				file = MemAllocNN(IO::Device::OlympusCameraControl::FileInfo);
				Text::StrConcat(file->fileName, sarr[1]);
				Text::StrConcat(file->filePath, sarr[0]);
				file->fileSize = Text::StrToUInt64(sarr[2]);
				dateVal = Text::StrToInt32(sarr[4]);
				timeVal = Text::StrToInt32(sarr[5]);
				dt.ToLocalTime();
				dt.SetValue((UInt16)(1980 + (dateVal >> 9)), (dateVal >> 5) & 15, dateVal & 31, timeVal >> 11, (timeVal >> 5) & 63, (timeVal & 31) << 1, 0);
				file->fileTimeTicks = dt.ToTicks();
				if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".MOV"))
				{
					file->fileType = IO::CameraControl::FT_MOVIE;
				}
				else
				{
					file->fileType = IO::CameraControl::FT_IMAGE;
				}
				this->fileList->Add(file);
			}
		}
		DEL_CLASS(reader);
		cli.Delete();
	}
}

void IO::Device::OlympusCameraControl::GetGPSLogList()
{
	if (this->cmdList.SortedIndexOfC(CSTR("get_gpsloglist")) < 0)
	{
		return;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	NN<IO::CameraControl::FileInfo> file;
	UTF8Char *sarr[11];
	Data::DateTime dt;
	Int32 dateVal;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/get_gpsloglist.cgi"));
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(sb, 1024))
		{
			break;
		}
		if (Text::StrSplit(sarr, 11, sb.v, ',') == 10)
		{
			if (Text::StrCharCnt(sarr[6]) == 8 && Text::StrCharCnt(sarr[7]) == 6)
			{
				file = MemAllocNN(IO::CameraControl::FileInfo);
				Text::StrConcat(file->fileName, sarr[1]);
				Text::StrConcat(file->filePath, sarr[0]);
				file->fileSize = Text::StrToUInt64(sarr[2]);
				dateVal = Text::StrToInt32(sarr[6]);
				dt.ToUTCTime();
				dt.SetValue((UInt16)(dateVal / 10000), (dateVal / 100) % 100, dateVal % 100, (sarr[7][0] - 48) * 10 + (sarr[7][1] - 48), (sarr[7][2] - 48) * 10 + (sarr[7][3] - 48), (sarr[7][4] - 48) * 10 + (sarr[7][5] - 48), 0);
				file->fileTimeTicks = dt.ToTicks();
				file->fileType = IO::CameraControl::FT_GPSLOG;
				this->fileList->Add(file);
			}
		}
	}
	DEL_CLASS(reader);
	cli.Delete();
}


void IO::Device::OlympusCameraControl::GetSNSLogList()
{
	if (this->cmdList.SortedIndexOfC(CSTR("get_snsloglist")) < 0)
	{
		return;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	NN<IO::CameraControl::FileInfo> file;
	UTF8Char *sarr[11];
	Data::DateTime dt;
	Int32 dateVal;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/get_snsloglist.cgi"));
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(sb, 1024))
		{
			break;
		}
		if (Text::StrSplit(sarr, 11, sb.v, ',') == 10)
		{
			if (Text::StrCharCnt(sarr[6]) == 8 && Text::StrCharCnt(sarr[7]) == 6)
			{
				file = MemAllocNN(IO::CameraControl::FileInfo);
				Text::StrConcat(file->fileName, sarr[1]);
				Text::StrConcat(file->filePath, sarr[0]);
				file->fileSize = Text::StrToUInt64(sarr[2]);
				dateVal = Text::StrToInt32(sarr[6]);
				dt.ToUTCTime();
				dt.SetValue((UInt16)(dateVal / 10000), (dateVal / 100) % 100, dateVal % 100, (sarr[7][0] - 48) * 10 + (sarr[7][1] - 48), (sarr[7][2] - 48) * 10 + (sarr[7][3] - 48), (sarr[7][4] - 48) * 10 + (sarr[7][5] - 48), 0);
				file->fileTimeTicks = dt.ToTicks();
				file->fileType = IO::CameraControl::FT_SENSORLOG;
				this->fileList->Add(file);
			}
		}
	}
	DEL_CLASS(reader);
	cli.Delete();
}

IO::Device::OlympusCameraControl::OlympusCameraControl(NN<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact, const Net::SocketUtil::AddressInfo *addr) : IO::CameraControl()
{
	this->addr = *addr;
	this->sockf = sockf;
	this->encFact = encFact;
	this->oiVersion = 0;
	this->oiTrackVersion = 0;
	this->fileList = 0;
	this->GetCommandList();
}

IO::Device::OlympusCameraControl::~OlympusCameraControl()
{
	this->cmdList.FreeAll();
	if (this->fileList)
	{
		NN<IO::CameraControl::FileInfo> file;
		UOSInt i = this->fileList->GetCount();
		while (i-- > 0)
		{
			file = this->fileList->GetItemNoCheck(i);
			MemFreeNN(file);
		}
		DEL_CLASS(this->fileList);
	}
	SDEL_STRING(this->oiVersion);
	SDEL_STRING(this->oiTrackVersion);
}

UOSInt IO::Device::OlympusCameraControl::GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList)
{
	Text::StringBuilderUTF8 sb;
	UOSInt initCnt = nameList->GetCount();
	if (this->GetModel(sb))
	{
		nameList->Add(Text::String::New(UTF8STRC("Model")));
		valueList->Add(Text::String::New(sb.ToString(), sb.GetLength()));
	}
	if (this->oiVersion)
	{
		nameList->Add(Text::String::New(UTF8STRC("OI Version")));
		valueList->Add(this->oiVersion->Clone());
	}
	if (this->oiTrackVersion)
	{
		nameList->Add(Text::String::New(UTF8STRC("OI Track Version")));
		valueList->Add(this->oiTrackVersion->Clone());
	}
	return nameList->GetCount() - initCnt;
}

void IO::Device::OlympusCameraControl::FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList)
{
	nameList->FreeAll();
	valueList->FreeAll();
}

UOSInt IO::Device::OlympusCameraControl::GetFileList(NN<Data::ArrayListNN<IO::Device::OlympusCameraControl::FileInfo>> fileList)
{
	if (this->fileList == 0)
	{
		this->GetImageList();
		this->GetGPSLogList();
		this->GetSNSLogList();
	}
	NN<Data::ArrayListNN<IO::Device::OlympusCameraControl::FileInfo>> nnfileList;
	if (nnfileList.Set(this->fileList))
	{
		fileList->AddAll(nnfileList);
		return nnfileList->GetCount();
	}
	return 0;
}

Bool IO::Device::OlympusCameraControl::GetFile(NN<IO::Device::OlympusCameraControl::FileInfo> file, NN<IO::Stream> outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UInt64 totalWriteSize = 0;
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	if (Text::StrEndsWith(file->fileName, (const UTF8Char*)".SNS"))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("/SNSLOG/"));
	}
	else if (Text::StrEndsWith(file->fileName, (const UTF8Char*)".LOG"))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("/GPSLOG/"));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("/DCIM/100OLYMP/"));
	}
	sptr = Text::StrConcat(sptr, file->fileName);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		totalWriteSize += outStm->Write(sbuff, readSize);
	}
	cli.Delete();
	return totalSize == file->fileSize && totalSize == totalWriteSize;
}

Bool IO::Device::OlympusCameraControl::GetThumbnailFile(NN<IO::Device::OlympusCameraControl::FileInfo> file, NN<IO::Stream> outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	if (Text::StrEndsWith(file->fileName, (const UTF8Char*)".SNS"))
	{
		return false;
	}
	else if (Text::StrEndsWith(file->fileName, (const UTF8Char*)".LOG"))
	{
		return false;
	}
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/get_thumbnail.cgi?DIR=/DCIM/100OLYMP/"));
	sptr = Text::StrConcat(sptr, file->fileName);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		outStm->Write(sbuff, readSize);
	}
	cli.Delete();
	return totalSize > 512;
}

Text::String *IO::Device::OlympusCameraControl::GetOIVersion()
{
	return this->oiVersion;
}

Text::String *IO::Device::OlympusCameraControl::GetOITrackVersion()
{
	return this->oiTrackVersion;
}

Bool IO::Device::OlympusCameraControl::GetModel(NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NN<Net::HTTPClient> cli;
	Bool found = false;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/get_caminfo.cgi"));
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);
	NN<Text::String> nodeText;
	while (reader.NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("caminfo")))
		{
			while (reader.NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("model")))
				{
					found = true;
					reader.ReadNodeText(sb);
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
		else
		{
			reader.SkipElement();
		}
	}
	cli.Delete();
	return found;
}

IO::Device::OlympusCameraControl *IO::Device::OlympusCameraControl::CreateControl(NN<Net::SocketFactory> sockf, Optional<Text::EncodingFactory> encFact)
{
	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	Bool found = false;
	if (sockf->GetConnInfoList(connInfoList) == 0)
		return 0;
	UInt32 ip = Net::SocketUtil::GetIPAddr(CSTR("192.168.0.10"));
	UOSInt i = connInfoList.GetCount();
	while (i-- > 0)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP && connInfo->GetDefaultGW() == ip)
		{
			found = true;
		}
		connInfo.Delete();
	}
	if (found)
	{
		Net::SocketUtil::AddressInfo addr;
		Net::SocketUtil::SetAddrInfoV4(addr, ip);
		IO::Device::OlympusCameraControl *ctrl;
		NEW_CLASS(ctrl, IO::Device::OlympusCameraControl(sockf, encFact, &addr));
		return ctrl;
	}
	return 0;
}

