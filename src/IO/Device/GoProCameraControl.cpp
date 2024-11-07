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
		UnsafeArray<UTF8Char> sptr;
		
		Text::StringBuilderUTF8 sb;
		NN<IO::CameraControl::FileInfo> file;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
		sptr = Net::SocketUtil::GetAddrName(sptr, this->addr).Or(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaList"));
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
		{
			Text::UTF8Reader reader(cli);
			reader.ReadToEnd(sb);
		}
		cli.Delete();
		NN<Text::JSONBase> jsBase;
		NN<Text::JSONObject> jsObj;
		NN<Text::JSONBase> jsBase2;
		NN<Text::JSONArray> jsArrDir;
		NN<Text::JSONObject> jsObjDir;
		NN<Text::JSONArray> jsArrFS;
		NN<Text::JSONObject> jsObjFS;
		Int64 timeDiff;
		Data::DateTime dt;
		dt.SetCurrTime();
		timeDiff = dt.GetTimeZoneQHR() * 15 * -60000LL;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jsBase))
		{
			if (jsBase->GetType() == Text::JSONType::Object)
			{
				jsObj = NN<Text::JSONObject>::ConvertFrom(jsBase);
				if (jsObj->GetObjectValue(CSTR("media")).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::Array)
				{
					jsArrDir = NN<Text::JSONArray>::ConvertFrom(jsBase2);
					i = 0;
					j = jsArrDir->GetArrayLength();
					while (i < j)
					{
						if (jsArrDir->GetArrayValue(i).SetTo(jsBase2) && jsBase->GetType() == Text::JSONType::Object)
						{
							NN<Text::String> dirName;
							jsObjDir = NN<Text::JSONObject>::ConvertFrom(jsBase2);

							if (jsObjDir->GetObjectString(CSTR("d")).SetTo(dirName) &&
								jsObjDir->GetObjectValue(CSTR("fs")).SetTo(jsBase2) &&
								jsBase2->GetType() == Text::JSONType::Array)
							{
								jsArrFS = NN<Text::JSONArray>::ConvertFrom(jsBase2);
								k = 0;
								l = jsArrFS->GetArrayLength();
								while (k < l)
								{
									if (jsArrFS->GetArrayValue(k).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::Object)
									{
										NN<Text::String> fileName;
										Optional<Text::String> modTime;
										NN<Text::String> fileSize;
										NN<Text::String> s;
										jsObjFS = NN<Text::JSONObject>::ConvertFrom(jsBase2);
										modTime = jsObjFS->GetObjectString(CSTR("mod"));
										
										if (jsObjFS->GetObjectString(CSTR("n")).SetTo(fileName) && jsObjFS->GetObjectString(CSTR("s")).SetTo(fileSize))
										{
											file = MemAllocNN(IO::CameraControl::FileInfo);
											file->fileNameLen = fileName->leng;
											fileName->ConcatTo(file->fileName2);
											file->filePathLen = dirName->leng;
											dirName->ConcatTo(file->filePath2);

											if (Text::StrEndsWithICaseC(file->fileName2, file->fileNameLen, UTF8STRC(".MP4")))
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
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr).Or(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/gp/gpControl/info"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	{
		Text::UTF8Reader reader(cli);
		reader.ReadToEnd(sb);
	}
	cli.Delete();
	NN<Text::JSONBase> jsBase;
	NN<Text::JSONObject> jsObj;
	NN<Text::JSONBase> jsBase2;
	NN<Text::JSONObject> jsInfo;
	if (Text::JSONBase::ParseJSONStr(sb.ToCString()).SetTo(jsBase))
	{
		if (jsBase->GetType() == Text::JSONType::Object)
		{
			jsObj = NN<Text::JSONObject>::ConvertFrom(jsBase);
			if (jsObj->GetObjectValue(CSTR("info")).SetTo(jsBase2) && jsBase->GetType() == Text::JSONType::Object)
			{
				jsInfo = NN<Text::JSONObject>::ConvertFrom(jsBase2);
				if (jsInfo->GetObjectValue(CSTR("model_name")).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Model")));
					valueList->Add(NN<Text::JSONString>::ConvertFrom(jsBase2)->GetValue()->Clone());
				}
				Double modelNo = jsInfo->GetObjectDoubleOrNAN(CSTR("model_number"));
				if (!Math::IsNAN(modelNo))
				{
					nameList->Add(Text::String::New(UTF8STRC("Model Number")));
					sb.ClearStr();
					sb.AppendDouble(modelNo);
					valueList->Add(Text::String::New(sb.ToCString()));
				}
				if (jsInfo->GetObjectValue(CSTR("firmware_version")).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Firmware Version")));
					valueList->Add(NN<Text::JSONString>::ConvertFrom(jsBase2)->GetValue()->Clone());
				}
				if (jsInfo->GetObjectValue(CSTR("serial_number")).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Serial Number")));
					valueList->Add(NN<Text::JSONString>::ConvertFrom(jsBase2)->GetValue()->Clone());
				}
				if (jsInfo->GetObjectValue(CSTR("board_type")).SetTo(jsBase2) && jsBase2->GetType() == Text::JSONType::String)
				{
					nameList->Add(Text::String::New(UTF8STRC("Board Type")));
					valueList->Add(NN<Text::JSONString>::ConvertFrom(jsBase2)->GetValue()->Clone());
				}
			}
		}
		jsBase->EndUse();
		return true;
	}
	return false;
}

IO::Device::GoProCameraControl::GoProCameraControl(NN<Net::TCPClientFactory> clif, const Net::SocketUtil::AddressInfo *addr) : IO::CameraControl()
{
	this->addr = *addr;
	this->clif = clif;
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
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr).Or(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/videos/DCIM/"));
	sptr = Text::StrConcatC(sptr, file->filePath2, file->filePathLen);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcatC(sptr, file->fileName2, file->fileNameLen);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		totalWriteSize += outStm->Write(Data::ByteArrayR(sbuff, readSize));
	}
	cli.Delete();
	return totalSize == file->fileSize && totalSize == totalWriteSize;
}

Bool IO::Device::GoProCameraControl::GetThumbnailFile(NN<IO::CameraControl::FileInfo> file, NN<IO::Stream> outStm)
{
	UTF8Char sbuff[2048];
	UOSInt readSize;
	UInt64 totalSize = 0;
	UnsafeArray<UTF8Char> sptr;
	if (!Text::StrStartsWithC(file->fileName2, file->fileNameLen, UTF8STRC("GOPR")))
	{
		return false;
	}
	sptr = Text::StrConcatC(sbuff, UTF8STRC("http://"));
	sptr = Net::SocketUtil::GetAddrName(sptr, this->addr).Or(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(":8080/gp/gpMediaMetadata?p="));
	sptr = Text::StrConcatC(sptr, file->filePath2, file->filePathLen);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrConcatC(sptr, file->fileName2, file->fileNameLen);
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, 0, CSTRP(sbuff, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	while ((readSize = cli->Read(BYTEARR(sbuff))) > 0)
	{
		totalSize += readSize;
		outStm->Write(Data::ByteArrayR(sbuff, readSize));
	}
	cli.Delete();
	return totalSize > 512;
}

Optional<IO::Device::GoProCameraControl> IO::Device::GoProCameraControl::CreateControl(NN<Net::TCPClientFactory> clif)
{
	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	Bool found = false;
	if (clif->GetSocketFactory()->GetConnInfoList(connInfoList) == 0)
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
		NEW_CLASS(ctrl, IO::Device::GoProCameraControl(clif, &addr));
		return ctrl;
	}
	return 0;
}

