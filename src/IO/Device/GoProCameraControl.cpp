#include "Stdafx.h"
#include "IO/Device/GoProCameraControl.h"
#include "Net/ConnectionInfo.h"
#include "Net/HTTPClient.h"
#include "Text/JSON.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"


void IO::Device::GoProCameraControl::GetMediaList()
{
	if (this->fileList == 0)
	{
		NEW_CLASS(this->fileList, Data::ArrayList<IO::CameraControl::FileInfo*>());
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Net::HTTPClient *cli;
		Text::UTF8Reader *reader;
		Text::StringBuilderUTF8 sb;
		IO::CameraControl::FileInfo *file;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
		sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaList"));
		cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		NEW_CLASS(reader, Text::UTF8Reader(cli));
		reader->ReadToEnd(&sb);
		DEL_CLASS(reader);
		DEL_CLASS(cli);
		Text::JSONBase *jsBase = Text::JSONBase::ParseJSONStr(sb.ToCString());
		Text::JSONObject *jsObj;
		Text::JSONBase *jsBase2;
		Text::JSONArray *jsArrDir;
		Text::JSONObject *jsObjDir;
		Text::JSONArray *jsArrFS;
		Text::JSONObject *jsObjFS;
		Text::String *dirName;
		Text::String *fileName;
		Text::String *modTime;
		Text::String *fileSize;
		Int64 timeDiff;
		Data::DateTime dt;
		dt.SetCurrTime();
		timeDiff = dt.GetTimeZoneQHR() * 15 * -60000LL;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		if (jsBase)
		{
			if (jsBase->GetType() == Text::JSONType::Object)
			{
				jsObj = (Text::JSONObject*)jsBase;
				jsBase2 = jsObj->GetObjectValue(CSTR("media"));
				if (jsBase2 && jsBase2->GetType() == Text::JSONType::Array)
				{
					jsArrDir = (Text::JSONArray*)jsBase2;
					i = 0;
					j = jsArrDir->GetArrayLength();
					while (i < j)
					{
						jsBase2 = jsArrDir->GetArrayValue(i);
						if (jsBase2 && jsBase->GetType() == Text::JSONType::Object)
						{
							jsObjDir = (Text::JSONObject *)jsBase2;
							dirName = jsObjDir->GetObjectString(CSTR("d"));

							jsBase2 = jsObjDir->GetObjectValue(CSTR("fs"));
							if (dirName && jsBase2 && jsBase2->GetType() == Text::JSONType::Array)
							{
								jsArrFS = (Text::JSONArray*)jsBase2;
								k = 0;
								l = jsArrFS->GetArrayLength();
								while (k < l)
								{
									jsBase2 = jsArrFS->GetArrayValue(k);
									if (jsBase2 && jsBase2->GetType() == Text::JSONType::Object)
									{
										jsObjFS = (Text::JSONObject*)jsBase2;
										fileName = jsObjFS->GetObjectString(CSTR("n"));
										modTime = jsObjFS->GetObjectString(CSTR("mod"));
										fileSize = jsObjFS->GetObjectString(CSTR("s"));
										
										if (fileName && fileSize)
										{
											file = MemAlloc(IO::CameraControl::FileInfo, 1);
											fileName->ConcatTo(file->fileName);
											dirName->ConcatTo(file->filePath);
											if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".MP4"))
											{
												file->fileType = IO::CameraControl::FT_MOVIE;
											}
											else
											{
												file->fileType = IO::CameraControl::FT_IMAGE;
											}
											file->fileSize = fileSize->ToUInt64();
											if (modTime == 0)
											{
												file->fileTimeTicks = 0;
											}
											else
											{
												file->fileTimeTicks = modTime->ToInt64() * 1000 + timeDiff;
											}
											this->fileList->Add(file);
										}
									}
									k++;
								}
							}
						}
						i++;
					}
				}
			}
			jsBase->EndUse();
		}
	}
}

Bool IO::Device::GoProCameraControl::GetInfo(Data::ArrayListNN<Text::String> *nameList, Data::ArrayListNN<Text::String> *valueList)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::HTTPClient *cli;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/gp/gpControl/info"));
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	reader->ReadToEnd(&sb);
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	Text::JSONBase *jsBase = Text::JSONBase::ParseJSONStr(sb.ToCString());
	Text::JSONObject *jsObj;
	Text::JSONBase *jsBase2;
	Text::JSONObject *jsInfo;
	if (jsBase)
	{
		if (jsBase->GetType() == Text::JSONType::Object)
		{
			jsObj = (Text::JSONObject*)jsBase;
			jsBase2 = jsObj->GetObjectValue(CSTR("info"));
			if (jsBase2 && jsBase->GetType() == Text::JSONType::Object)
			{
				jsInfo = (Text::JSONObject*)jsBase2;
				jsBase2 = jsInfo->GetObjectValue(CSTR("model_name"));
				if (jsBase2 && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Model")));
					valueList->Add(((Text::JSONString*)jsBase2)->GetValue()->Clone());
				}
				Double modelNo = jsInfo->GetObjectDouble(CSTR("model_number"));
				if (modelNo != 0)
				{
					nameList->Add(Text::String::New(UTF8STRC("Model Number")));
					sb.ClearStr();
					Text::SBAppendF64(&sb, modelNo);
					valueList->Add(Text::String::New(sb.ToCString()));
				}
				jsBase2 = jsInfo->GetObjectValue(CSTR("firmware_version"));
				if (jsBase2 && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Firmware Version")));
					valueList->Add(((Text::JSONString*)jsBase2)->GetValue()->Clone());
				}
				jsBase2 = jsInfo->GetObjectValue(CSTR("serial_number"));
				if (jsBase2 && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Serial Number")));
					valueList->Add(((Text::JSONString*)jsBase2)->GetValue()->Clone());
				}
				jsBase2 = jsInfo->GetObjectValue(CSTR("board_type"));
				if (jsBase2 && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Board Type")));
					valueList->Add(((Text::JSONString*)jsBase2)->GetValue()->Clone());
				}
			}
		}
		jsBase->EndUse();
		return true;
	}
	return false;
}

IO::Device::GoProCameraControl::GoProCameraControl(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr) : IO::CameraControl()
{
	this->addr = *addr;
	this->sockf = sockf;
	this->fileList = 0;
}

IO::Device::GoProCameraControl::~GoProCameraControl()
{
	UOSInt i;
	if (this->fileList)
	{
		IO::CameraControl::FileInfo *file;
		i = this->fileList->GetCount();
		while (i-- > 0)
		{
			file = this->fileList->GetItem(i);
			MemFree(file);
		}
		DEL_CLASS(this->fileList);
	}
}

UOSInt IO::Device::GoProCameraControl::GetInfoList(Data::ArrayListNN<Text::String> *nameList, Data::ArrayListNN<Text::String> *valueList)
{
	Text::StringBuilderUTF8 sb;
	UOSInt initCnt = nameList->GetCount();
	this->GetInfo(nameList, valueList);
	return nameList->GetCount() - initCnt;
}

void IO::Device::GoProCameraControl::FreeInfoList(Data::ArrayListNN<Text::String> *nameList, Data::ArrayListNN<Text::String> *valueList)
{
	UOSInt i = nameList->GetCount();
	while (i-- > 0)
	{
		nameList->GetItem(i)->Release();
	}
	nameList->Clear();
	i = valueList->GetCount();
	while (i-- > 0)
	{
		valueList->GetItem(i)->Release();
	}
	valueList->Clear();
}

UOSInt IO::Device::GoProCameraControl::GetFileList(Data::ArrayList<IO::CameraControl::FileInfo*> *fileList)
{
	if (this->fileList == 0)
	{
		this->GetMediaList();
	}
	fileList->AddAll(this->fileList);
	return this->fileList->GetCount();
}

Bool IO::Device::GoProCameraControl::GetFile(IO::CameraControl::FileInfo *file, IO::Stream *outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UInt64 totalWriteSize = 0;
	UTF8Char *sptr;
	Net::HTTPClient *cli;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/videos/DCIM/"));
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcat(sptr, file->fileName);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(sbuff, 2048)) > 0)
	{
		totalSize += readSize;
		totalWriteSize += outStm->Write(sbuff, readSize);
	}
	DEL_CLASS(cli);
	return totalSize == file->fileSize && totalSize == totalWriteSize;
}

Bool IO::Device::GoProCameraControl::GetThumbnailFile(IO::CameraControl::FileInfo *file, IO::Stream *outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UTF8Char *sptr;
	Net::HTTPClient *cli;
	UOSInt nameLen = Text::StrCharCnt(file->fileName);
	if (!Text::StrStartsWithC(file->fileName, nameLen, UTF8STRC("GOPR")))
	{
		return false;
	}
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaMetadata?p="));
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcatC(sptr, file->fileName, nameLen);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(sbuff, 2048)) > 0)
	{
		totalSize += readSize;
		outStm->Write(sbuff, readSize);
	}
	DEL_CLASS(cli);
	return totalSize > 512;
}

IO::Device::GoProCameraControl *IO::Device::GoProCameraControl::CreateControl(Net::SocketFactory *sockf)
{
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	Bool found = false;
	if (sockf->GetConnInfoList(&connInfoList) == 0)
		return 0;
	UInt32 ip = Net::SocketUtil::GetIPAddr(CSTR("10.5.5.9"));
	UOSInt i = connInfoList.GetCount();
	while (i-- > 0)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP && connInfo->GetDefaultGW() == ip)
		{
			found = true;
		}
		DEL_CLASS(connInfo);
	}
	if (found)
	{
		Net::SocketUtil::AddressInfo addr;
		Net::SocketUtil::SetAddrInfoV4(&addr, ip);
		IO::Device::GoProCameraControl *ctrl;
		NEW_CLASS(ctrl, IO::Device::GoProCameraControl(sockf, &addr));
		return ctrl;
	}
	return 0;
}

