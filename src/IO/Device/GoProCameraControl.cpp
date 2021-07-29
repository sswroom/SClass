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
		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"http://");
		sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)":8080/gp/gpMediaList");
		cli = Net::HTTPClient::CreateConnect(this->sockf, 0, sbuff, "GET", true);
		NEW_CLASS(reader, Text::UTF8Reader(cli));
		reader->ReadToEnd(&sb);
		DEL_CLASS(reader);
		DEL_CLASS(cli);
		Text::JSONBase *jsBase = Text::JSONBase::ParseJSONStrLen(sb.ToString(), sb.GetLength());
		Text::JSONObject *jsObj;
		Text::JSONBase *jsBase2;
		Text::JSONArray *jsArrDir;
		Text::JSONObject *jsObjDir;
		Text::JSONArray *jsArrFS;
		Text::JSONObject *jsObjFS;
		const UTF8Char *dirName;
		const UTF8Char *fileName;
		const UTF8Char *modTime;
		const UTF8Char *fileSize;
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
			if (jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
			{
				jsObj = (Text::JSONObject*)jsBase;
				jsBase2 = jsObj->GetObjectValue((const UTF8Char *)"media");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_ARRAY)
				{
					jsArrDir = (Text::JSONArray*)jsBase2;
					i = 0;
					j = jsArrDir->GetArrayLength();
					while (i < j)
					{
						jsBase2 = jsArrDir->GetArrayValue(i);
						if (jsBase2 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
						{
							jsObjDir = (Text::JSONObject *)jsBase2;
							jsBase2 = jsObjDir->GetObjectValue((const UTF8Char*)"d");
							if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
							{
								dirName = ((Text::JSONStringUTF8*)jsBase2)->GetValue();
							}
							else
							{
								dirName = 0;
							}

							jsBase2 = jsObjDir->GetObjectValue((const UTF8Char*)"fs");
							if (dirName && jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_ARRAY)
							{
								jsArrFS = (Text::JSONArray*)jsBase2;
								k = 0;
								l = jsArrFS->GetArrayLength();
								while (k < l)
								{
									jsBase2 = jsArrFS->GetArrayValue(k);
									if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_OBJECT)
									{
										jsObjFS = (Text::JSONObject*)jsBase2;
										jsBase2 = jsObjFS->GetObjectValue((const UTF8Char*)"n");
										if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
										{
											fileName = ((Text::JSONStringUTF8*)jsBase2)->GetValue();
										}
										else
										{
											fileName = 0;
										}
										jsBase2 = jsObjFS->GetObjectValue((const UTF8Char*)"mod");
										if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
										{
											modTime = ((Text::JSONStringUTF8*)jsBase2)->GetValue();
										}
										else
										{
											modTime = 0;
										}
										jsBase2 = jsObjFS->GetObjectValue((const UTF8Char*)"s");
										if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
										{
											fileSize = ((Text::JSONStringUTF8*)jsBase2)->GetValue();
										}
										else
										{
											fileSize = 0;
										}
										
										if (fileName && fileSize)
										{
											file = MemAlloc(IO::CameraControl::FileInfo, 1);
											Text::StrConcat(file->fileName, fileName);
											Text::StrConcat(file->filePath, dirName);
											if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".MP4"))
											{
												file->fileType = IO::CameraControl::FT_MOVIE;
											}
											else
											{
												file->fileType = IO::CameraControl::FT_IMAGE;
											}
											file->fileSize = Text::StrToUInt64(fileSize);
											if (modTime == 0)
											{
												file->fileTimeTicks = 0;
											}
											else
											{
												file->fileTimeTicks = Text::StrToInt64(modTime) * 1000 + timeDiff;
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

Bool IO::Device::GoProCameraControl::GetInfo(Data::ArrayList<const UTF8Char*> *nameList, Data::ArrayList<const UTF8Char*> *valueList)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::HTTPClient *cli;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"http://");
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/gp/gpControl/info");
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, sbuff, "GET", true);
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	reader->ReadToEnd(&sb);
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	Text::JSONBase *jsBase = Text::JSONBase::ParseJSONStrLen(sb.ToString(), sb.GetLength());
	Text::JSONObject *jsObj;
	Text::JSONBase *jsBase2;
	Text::JSONObject *jsInfo;
	if (jsBase)
	{
		if (jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
		{
			jsObj = (Text::JSONObject*)jsBase;
			jsBase2 = jsObj->GetObjectValue((const UTF8Char *)"info");
			if (jsBase2 && jsBase->GetJSType() == Text::JSONBase::JST_OBJECT)
			{
				jsInfo = (Text::JSONObject*)jsBase2;
				jsBase2 = jsInfo->GetObjectValue((const UTF8Char*)"model_name");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
				{
					nameList->Add(Text::StrCopyNew((const UTF8Char*)"Model"));
					valueList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase2)->GetValue()));
				}
				jsBase2 = jsInfo->GetObjectValue((const UTF8Char*)"model_number");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_NUMBER)
				{
					nameList->Add(Text::StrCopyNew((const UTF8Char*)"Model Number"));
					sb.ClearStr();
					Text::SBAppendF64(&sb, ((Text::JSONNumber*)jsBase2)->GetValue());
					valueList->Add(Text::StrCopyNew(sb.ToString()));
				}
				jsBase2 = jsInfo->GetObjectValue((const UTF8Char*)"firmware_version");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
				{
					nameList->Add(Text::StrCopyNew((const UTF8Char*)"Firmware Version"));
					valueList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase2)->GetValue()));
				}
				jsBase2 = jsInfo->GetObjectValue((const UTF8Char*)"serial_number");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
				{
					nameList->Add(Text::StrCopyNew((const UTF8Char*)"Serial Number"));
					valueList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase2)->GetValue()));
				}
				jsBase2 = jsInfo->GetObjectValue((const UTF8Char*)"board_type");
				if (jsBase2 && jsBase2->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
				{
					nameList->Add(Text::StrCopyNew((const UTF8Char*)"Board Type"));
					valueList->Add(Text::StrCopyNew(((Text::JSONStringUTF8*)jsBase2)->GetValue()));
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

UOSInt IO::Device::GoProCameraControl::GetInfoList(Data::ArrayList<const UTF8Char*> *nameList, Data::ArrayList<const UTF8Char*> *valueList)
{
	Text::StringBuilderUTF8 sb;
	UOSInt initCnt = nameList->GetCount();
	this->GetInfo(nameList, valueList);
	return nameList->GetCount() - initCnt;
}

void IO::Device::GoProCameraControl::FreeInfoList(Data::ArrayList<const UTF8Char*> *nameList, Data::ArrayList<const UTF8Char*> *valueList)
{
	UOSInt i = nameList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(nameList->GetItem(i));
	}
	nameList->Clear();
	i = valueList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(valueList->GetItem(i));
	}
	valueList->Clear();
}

UOSInt IO::Device::GoProCameraControl::GetFileList(Data::ArrayList<IO::CameraControl::FileInfo*> *fileList)
{
	if (this->fileList == 0)
	{
		this->GetMediaList();
	}
	fileList->AddRange(this->fileList);
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
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"http://");
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)":8080/videos/DCIM/");
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/");
	sptr = Text::StrConcat(sptr, file->fileName);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, sbuff, "GET", true);
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
	if (!Text::StrStartsWith(file->fileName, (const UTF8Char*)"GOPR"))
	{
		return false;
	}
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"http://");
	sptr = Net::SocketUtil::GetAddrName(sptr, &this->addr);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)":8080/gp/gpMediaMetadata?p=");
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/");
	sptr = Text::StrConcat(sptr, file->fileName);
	cli = Net::HTTPClient::CreateConnect(this->sockf, 0, sbuff, "GET", true);
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
	UInt32 ip = Net::SocketUtil::GetIPAddr((const UTF8Char*)"10.5.5.9");
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

