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
		NEW_CLASS(this->fileList, Data::ArrayListNN<IO::CameraControl::FileInfo>());
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		
		Text::StringBuilderUTF8 sb;
		NN<IO::CameraControl::FileInfo> file;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
		sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaList"));
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		{
			Text::UTF8Reader reader(cli);
			reader.ReadToEnd(sb);
		}
		cli.Delete();
		Text::JSONBase *jsBase = Text::JSONBase::ParseJSONStr(sb.ToCString());
		Text::JSONObject *jsObj;
		Text::JSONBase *jsBase2;
		Text::JSONArray *jsArrDir;
		Text::JSONObject *jsObjDir;
		Text::JSONArray *jsArrFS;
		Text::JSONObject *jsObjFS;
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
							NN<Text::String> dirName;
							jsObjDir = (Text::JSONObject *)jsBase2;

							jsBase2 = jsObjDir->GetObjectValue(CSTR("fs"));
							if (jsObjDir->GetObjectString(CSTR("d")).SetTo(dirName) && jsBase2 && jsBase2->GetType() == Text::JSONType::Array)
							{
								jsArrFS = (Text::JSONArray*)jsBase2;
								k = 0;
								l = jsArrFS->GetArrayLength();
								while (k < l)
								{
									jsBase2 = jsArrFS->GetArrayValue(k);
									if (jsBase2 && jsBase2->GetType() == Text::JSONType::Object)
									{
										NN<Text::String> fileName;
										Optional<Text::String> modTime;
										NN<Text::String> fileSize;
										NN<Text::String> s;
										jsObjFS = (Text::JSONObject*)jsBase2;
										modTime = jsObjFS->GetObjectString(CSTR("mod"));
										
										if (jsObjFS->GetObjectString(CSTR("n")).SetTo(fileName) && jsObjFS->GetObjectString(CSTR("s")).SetTo(fileSize))
										{
											file = MemAllocNN(IO::CameraControl::FileInfo);
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
											if (!modTime.SetTo(s))
											{
												file->fileTimeTicks = 0;
											}
											else
											{
												file->fileTimeTicks = s->ToInt64() * 1000 + timeDiff;
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

Bool IO::Device::GoProCameraControl::GetInfo(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/gp/gpControl/info"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	{
		Text::UTF8Reader reader(cli);
		reader.ReadToEnd(sb);
	}
	cli.Delete();
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
					sb.AppendDouble(modelNo);
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

IO::Device::GoProCameraControl::GoProCameraControl(NN<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *addr) : IO::CameraControl()
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
		NN<IO::CameraControl::FileInfo> file;
		i = this->fileList->GetCount();
		while (i-- > 0)
		{
			file = this->fileList->GetItemNoCheck(i);
			MemFreeNN(file);
		}
		DEL_CLASS(this->fileList);
	}
}

UOSInt IO::Device::GoProCameraControl::GetInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList)
{
	Text::StringBuilderUTF8 sb;
	UOSInt initCnt = nameList->GetCount();
	this->GetInfo(nameList, valueList);
	return nameList->GetCount() - initCnt;
}

void IO::Device::GoProCameraControl::FreeInfoList(NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListStringNN> valueList)
{
	nameList->FreeAll();
	valueList->FreeAll();
}

UOSInt IO::Device::GoProCameraControl::GetFileList(NN<Data::ArrayListNN<IO::CameraControl::FileInfo>> fileList)
{
	if (this->fileList == 0)
	{
		this->GetMediaList();
	}
	NN<Data::ArrayListNN<IO::CameraControl::FileInfo>> nnfileList;
	if (nnfileList.Set(this->fileList))
	{
		fileList->AddAll(nnfileList);
		return nnfileList->GetCount();
	}
	return 0;
}

Bool IO::Device::GoProCameraControl::GetFile(NN<IO::CameraControl::FileInfo> file, NN<IO::Stream> outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UInt64 totalWriteSize = 0;
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/videos/DCIM/"));
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcat(sptr, file->fileName);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		totalWriteSize += outStm->Write(sbuff, readSize);
	}
	cli.Delete();
	return totalSize == file->fileSize && totalSize == totalWriteSize;
}

Bool IO::Device::GoProCameraControl::GetThumbnailFile(NN<IO::CameraControl::FileInfo> file, NN<IO::Stream> outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UTF8Char *sptr;
	UOSInt nameLen = Text::StrCharCnt(file->fileName);
	if (!Text::StrStartsWithC(file->fileName, nameLen, UTF8STRC("GOPR")))
	{
		return false;
	}
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaMetadata?p="));
	sptr = Text::StrConcat(sptr, file->filePath);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcatC(sptr, file->fileName, nameLen);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		outStm->Write(sbuff, readSize);
	}
	cli.Delete();
	return totalSize > 512;
}

IO::Device::GoProCameraControl *IO::Device::GoProCameraControl::CreateControl(NN<Net::SocketFactory> sockf)
{
	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	Bool found = false;
	if (sockf->GetConnInfoList(connInfoList) == 0)
		return 0;
	UInt32 ip = Net::SocketUtil::GetIPAddr(CSTR("10.5.5.9"));
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
		IO::Device::GoProCameraControl *ctrl;
		NEW_CLASS(ctrl, IO::Device::GoProCameraControl(sockf, &addr));
		return ctrl;
	}
	return 0;
}

