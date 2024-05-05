#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "Media/ColorManager.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "Media/ICCProfile.h"
#include "Media/MonitorInfo.h"
#include "Sync/MutexUsage.h"
#include "Sync/RWMutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringTool.h"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

void Media::MonitorColorManager::SetDefaultYUV(NN<Media::IColorHandler::YUVPARAM> yuv)
{
	yuv->Brightness = 1;
	yuv->Contrast = 1;
	yuv->Saturation = 1;
	yuv->YGamma = 1;
	yuv->CGamma = 1;

	yuv->RAdd = 0;
	yuv->RMul = 1;

	yuv->GAdd = 0;
	yuv->GMul = 1;
	
	yuv->BAdd = 0;
	yuv->BMul = 1;
}

void Media::MonitorColorManager::SetDefaultRGB(NN<Media::IColorHandler::RGBPARAM2> rgb)
{
	rgb->MonRBright = 1;
	rgb->MonRContr = 1;
	rgb->MonRGamma = 1;

	rgb->MonGBright = 1;
	rgb->MonGContr = 1;
	rgb->MonGGamma = 1;

	rgb->MonBBright = 1;
	rgb->MonBContr = 1;
	rgb->MonBGamma = 1;

	rgb->MonVBrightness = 1;
	rgb->MonPBrightness = 1;
	rgb->MonRBrightness = 1;
	rgb->MonGBrightness = 1;
	rgb->MonBBrightness = 1;

	rgb->monProfileType = Media::ColorProfile::CPT_EDID;
	rgb->monProfile.SetCommonProfile(rgb->monProfileType);
	rgb->monLuminance = 250.0;
}

Media::MonitorColorManager::MonitorColorManager(Optional<Text::String> profileName)
{
	this->profileName = Text::String::CopyOrNull(profileName);
	this->monProfileFile = 0;
	this->SetDefault();
	this->Load();
}

Media::MonitorColorManager::MonitorColorManager(Text::CString profileName)
{
	this->profileName = Text::String::NewOrNull(profileName);
	this->monProfileFile = 0;
	this->SetDefault();
	this->Load();
}

Media::MonitorColorManager::~MonitorColorManager()
{
	UOSInt i;
	i = this->sessList.GetCount();
	while (i-- > 0)
	{
		this->sessList.GetItem(i).Delete();
	}
	OPTSTR_DEL(this->monProfileFile);
	OPTSTR_DEL(this->profileName);
}

Optional<Text::String> Media::MonitorColorManager::GetProfileName()
{
	return this->profileName;
}

Bool Media::MonitorColorManager::Load()
{
	IO::Registry *regBase;
	IO::Registry *reg2;
	IO::Registry *reg;
	NN<Text::String> s;
	WChar wbuff[512];

	regBase = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"Color");
	if (regBase)
	{
		reg2 = 0;
		reg = regBase;
		if (this->profileName.SetTo(s))
		{
			const WChar *wptr = Text::StrToWCharNew(s->v);
			reg2 = reg->OpenSubReg(wptr);
			Text::StrDelNew(wptr);
			if (reg2 == 0)
			{
				IO::Registry::CloseRegistry(regBase);
				return false;
			}
			reg = reg2;
		}

		Int32 tmpVal;

		if (reg->GetValueI32(L"MonRB", tmpVal))
			this->rgb.MonRBright = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonRC", tmpVal))
			this->rgb.MonRContr = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonRG", tmpVal))
			this->rgb.MonRGamma = tmpVal * 0.001;

		if (reg->GetValueI32(L"MonGB", tmpVal))
			this->rgb.MonGBright = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonGC", tmpVal))
			this->rgb.MonGContr = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonGG", tmpVal))
			this->rgb.MonGGamma = tmpVal * 0.001;

		if (reg->GetValueI32(L"MonBB", tmpVal))
			this->rgb.MonBBright = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonBC", tmpVal))
			this->rgb.MonBContr = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonBG", tmpVal))
			this->rgb.MonBGamma = tmpVal * 0.001;

		if (reg->GetValueI32(L"MonBright", tmpVal))
			this->rgb.MonVBrightness = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonPBright", tmpVal))
			this->rgb.MonPBrightness = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonRBright", tmpVal))
			this->rgb.MonRBrightness = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonGBright", tmpVal))
			this->rgb.MonGBrightness = tmpVal * 0.001;
		if (reg->GetValueI32(L"MonBBright", tmpVal))
			this->rgb.MonBBrightness = tmpVal * 0.001;

		if (reg->GetValueI32(L"MonTransfer", tmpVal))
		{
			this->rgb.monProfile.GetRTranParam()->Set((Media::CS::TransferType)tmpVal, 2.2);
			this->rgb.monProfile.GetGTranParam()->Set((Media::CS::TransferType)tmpVal, 2.2);
			this->rgb.monProfile.GetBTranParam()->Set((Media::CS::TransferType)tmpVal, 2.2);
		}
		if (reg->GetValueI32(L"MonRX", tmpVal))
			this->rgb.monProfile.GetPrimaries()->r.x = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonRY", tmpVal))
			this->rgb.monProfile.GetPrimaries()->r.y = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonGX", tmpVal))
			this->rgb.monProfile.GetPrimaries()->g.x = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonGY", tmpVal))
			this->rgb.monProfile.GetPrimaries()->g.y = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonBX", tmpVal))
			this->rgb.monProfile.GetPrimaries()->b.x = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonBY", tmpVal))
			this->rgb.monProfile.GetPrimaries()->b.y = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonWX", tmpVal))
			this->rgb.monProfile.GetPrimaries()->w.x = tmpVal * 0.000000001;
		if (reg->GetValueI32(L"MonWY", tmpVal))
			this->rgb.monProfile.GetPrimaries()->w.y = tmpVal * 0.000000001;

		if (reg->GetValueI32(L"MonLuminance", tmpVal))
			this->rgb.monLuminance = tmpVal * 0.1;

		if (reg->GetValueStr(L"MonProfileFile", wbuff))
		{
			OPTSTR_DEL(this->monProfileFile);
			this->monProfileFile = Text::String::NewNotNull(wbuff);
		}
		if (reg->GetValueI32(L"MonProfileType", tmpVal))
		{
			this->rgb.monProfileType = (Media::ColorProfile::CommonProfileType)tmpVal;
			NN<Text::String> fileName;
			if (this->rgb.monProfileType == Media::ColorProfile::CPT_FILE && this->monProfileFile.SetTo(fileName))
			{
				if (!SetFromProfileFile(fileName))
				{
					this->rgb.monProfile.SetCommonProfile(this->rgb.monProfileType);
				}
			}
			else if (this->rgb.monProfileType == Media::ColorProfile::CPT_OS)
			{
				this->SetOSProfile();
			}
			else if (this->rgb.monProfileType == Media::ColorProfile::CPT_EDID)
			{
				this->SetEDIDProfile();
			}
			else
			{
				this->rgb.monProfile.SetCommonProfile(this->rgb.monProfileType);
			}
		}

		if (reg->GetValueI32(L"Brightness", tmpVal))
			this->yuv.Brightness = tmpVal * 0.001;
		if (reg->GetValueI32(L"Contrast", tmpVal))
			this->yuv.Contrast = tmpVal * 0.001;
		if (reg->GetValueI32(L"Saturation", tmpVal))
			this->yuv.Saturation = tmpVal * 0.001;
		if (reg->GetValueI32(L"YGamma", tmpVal))
			this->yuv.YGamma = tmpVal * 0.001;
		if (reg->GetValueI32(L"CGamma", tmpVal))
			this->yuv.CGamma = tmpVal * 0.001;
		if (reg->GetValueI32(L"Color10Bit", tmpVal))
			this->color10Bit = (tmpVal != 0);

		IO::Registry::CloseRegistry(regBase);
		if (reg2)
		{
			IO::Registry::CloseRegistry(reg2);
		}
		this->RGBUpdated();
		this->YUVUpdated();
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::MonitorColorManager::Save()
{
	IO::Registry *regBase;
	IO::Registry *reg2;
	IO::Registry *reg;
	NN<Text::String> s;

	regBase = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"Color");
	if (regBase)
	{
		reg2 = 0;
		reg = regBase;
		if (this->profileName.SetTo(s))
		{
			const WChar *wptr = Text::StrToWCharNew(s->v);
			reg2 = reg->OpenSubReg(wptr);
			Text::StrDelNew(wptr);
			if (reg2 == 0)
			{
				IO::Registry::CloseRegistry(regBase);
				return false;
			}
			reg = reg2;
		}

		reg->SetValue(L"MonRB", Double2Int32(this->rgb.MonRBright * 1000.0));
		reg->SetValue(L"MonRC", Double2Int32(this->rgb.MonRContr * 1000.0));
		reg->SetValue(L"MonRG", Double2Int32(this->rgb.MonRGamma * 1000.0));
		
		reg->SetValue(L"MonGB", Double2Int32(this->rgb.MonGBright * 1000.0));
		reg->SetValue(L"MonGC", Double2Int32(this->rgb.MonGContr * 1000.0));
		reg->SetValue(L"MonGG", Double2Int32(this->rgb.MonGGamma * 1000.0));

		reg->SetValue(L"MonBB", Double2Int32(this->rgb.MonBBright * 1000.0));
		reg->SetValue(L"MonBC", Double2Int32(this->rgb.MonBContr * 1000.0));
		reg->SetValue(L"MonBG", Double2Int32(this->rgb.MonBGamma * 1000.0));

		reg->SetValue(L"MonBright", Double2Int32(this->rgb.MonVBrightness * 1000.0));
		reg->SetValue(L"MonPBright", Double2Int32(this->rgb.MonPBrightness * 1000.0));
		reg->SetValue(L"MonRBright", Double2Int32(this->rgb.MonRBrightness * 1000.0));
		reg->SetValue(L"MonGBright", Double2Int32(this->rgb.MonGBrightness * 1000.0));
		reg->SetValue(L"MonBBright", Double2Int32(this->rgb.MonBBrightness * 1000.0));

		reg->SetValue(L"MonTransfer", (Int32)this->rgb.monProfile.GetRTranParam()->GetTranType());
		reg->SetValue(L"MonRX", Double2Int32(this->rgb.monProfile.GetPrimaries()->r.x * 1000000000.0));
		reg->SetValue(L"MonRY", Double2Int32(this->rgb.monProfile.GetPrimaries()->r.y * 1000000000.0));
		reg->SetValue(L"MonGX", Double2Int32(this->rgb.monProfile.GetPrimaries()->g.x * 1000000000.0));
		reg->SetValue(L"MonGY", Double2Int32(this->rgb.monProfile.GetPrimaries()->g.y * 1000000000.0));
		reg->SetValue(L"MonBX", Double2Int32(this->rgb.monProfile.GetPrimaries()->b.x * 1000000000.0));
		reg->SetValue(L"MonBY", Double2Int32(this->rgb.monProfile.GetPrimaries()->b.y * 1000000000.0));
		reg->SetValue(L"MonWX", Double2Int32(this->rgb.monProfile.GetPrimaries()->w.x * 1000000000.0));
		reg->SetValue(L"MonWY", Double2Int32(this->rgb.monProfile.GetPrimaries()->w.y * 1000000000.0));

		reg->SetValue(L"MonProfileType", (Int32)this->rgb.monProfileType);
		NN<Text::String> monProfileFile;
		if (this->monProfileFile.SetTo(monProfileFile))
		{
			const WChar *wptr = Text::StrToWCharNew(monProfileFile->v);
			reg->SetValue(L"MonProfileFile", wptr);
			Text::StrDelNew(wptr);
		}
		reg->SetValue(L"MonLuminance", Double2Int32(this->rgb.monLuminance * 10.0));

		reg->SetValue(L"Brightness", Double2Int32(this->yuv.Brightness * 1000.0));
		reg->SetValue(L"Contrast", Double2Int32(this->yuv.Contrast * 1000.0));
		reg->SetValue(L"Saturation", Double2Int32(this->yuv.Saturation * 1000.0));
		reg->SetValue(L"YGamma", Double2Int32(this->yuv.YGamma * 1000.0));
		reg->SetValue(L"CGamma", Double2Int32(this->yuv.CGamma * 1000.0));
		reg->SetValue(L"Color10Bit", this->color10Bit?1:0);

		IO::Registry::CloseRegistry(regBase);
		if (reg2)
		{
			IO::Registry::CloseRegistry(reg2);
		}
		this->RGBUpdated();
		return true;
	}
	else
	{
		return false;
	}
}

void Media::MonitorColorManager::SetDefault()
{
	SetDefaultRGB(this->rgb);
	SetDefaultYUV(this->yuv);
	this->color10Bit = false;
	if (this->rgb.monProfileType == Media::ColorProfile::CPT_EDID)
	{
		this->SetEDIDProfile();
	}
	else if (this->rgb.monProfileType == Media::ColorProfile::CPT_OS)
	{
		this->SetOSProfile();
	}
}

NN<const Media::IColorHandler::YUVPARAM> Media::MonitorColorManager::GetYUVParam()
{
	return this->yuv;
}

NN<const Media::IColorHandler::RGBPARAM2> Media::MonitorColorManager::GetRGBParam()
{
	return this->rgb;
}

void Media::MonitorColorManager::SetMonVBright(Double newVal)
{
	if (newVal != this->rgb.MonVBrightness && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonVBrightness = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetMonPBright(Double newVal)
{
	if (newVal != this->rgb.MonPBrightness && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonPBrightness = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetMonRBright(Double newVal)
{
	if (newVal != this->rgb.MonRBrightness && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonRBrightness = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetMonGBright(Double newVal)
{
	if (newVal != this->rgb.MonGBrightness && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonGBrightness = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetMonBBright(Double newVal)
{
	if (newVal != this->rgb.MonBBrightness && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonBBrightness = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetRMonBright(Double newVal)
{
	if (newVal != this->rgb.MonRBright && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonRBright = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetRMonContr(Double newVal)
{
	if (newVal != this->rgb.MonRContr && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonRContr = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetRMonGamma(Double newVal)
{
	if (newVal != this->rgb.MonRGamma && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonRGamma = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetGMonBright(Double newVal)
{
	if (newVal != this->rgb.MonGBright && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonGBright = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetGMonContr(Double newVal)
{
	if (newVal != this->rgb.MonGContr && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonGContr = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetGMonGamma(Double newVal)
{
	if (newVal != this->rgb.MonGGamma && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonGGamma = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetBMonBright(Double newVal)
{
	if (newVal != this->rgb.MonBBright && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonBBright = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetBMonContr(Double newVal)
{
	if (newVal != this->rgb.MonBContr && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonBContr = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetBMonGamma(Double newVal)
{
	if (newVal != this->rgb.MonBGamma && newVal >= 0 && newVal <= 4)
	{
		this->rgb.MonBGamma = newVal;
		this->RGBUpdated();
	}
}

void Media::MonitorColorManager::SetYUVBright(Double newVal)
{
	if (newVal != this->yuv.Brightness && newVal >= 0 && newVal <= 4)
	{
		this->yuv.Brightness = newVal;
		this->YUVUpdated();
	}
}

void Media::MonitorColorManager::SetYUVContr(Double newVal)
{
	if (newVal != this->yuv.Contrast && newVal >= 0 && newVal <= 4)
	{
		this->yuv.Contrast = newVal;
		this->YUVUpdated();
	}
}

void Media::MonitorColorManager::SetYUVSat(Double newVal)
{
	if (newVal != this->yuv.Saturation && newVal >= 0 && newVal <= 4)
	{
		this->yuv.Saturation = newVal;
		this->YUVUpdated();
	}
}

void Media::MonitorColorManager::SetYGamma(Double newVal)
{
	if (newVal != this->yuv.YGamma && newVal >= 0 && newVal <= 4)
	{
		this->yuv.YGamma = newVal;
		this->YUVUpdated();
	}
}

void Media::MonitorColorManager::SetCGamma(Double newVal)
{
	if (newVal != this->yuv.CGamma && newVal >= 0 && newVal <= 4)
	{
		this->yuv.CGamma = newVal;
		this->YUVUpdated();
	}
}

void Media::MonitorColorManager::SetMonProfileType(Media::ColorProfile::CommonProfileType newVal)
{
	if (newVal != this->rgb.monProfileType)
	{
		this->rgb.monProfileType = newVal;
		NN<Text::String> fileName;
		if (this->rgb.monProfileType == Media::ColorProfile::CPT_FILE && this->monProfileFile.SetTo(fileName))
		{
			if (SetFromProfileFile(fileName))
			{
				this->RGBUpdated();
			}
		}
		else if (this->rgb.monProfileType == Media::ColorProfile::CPT_OS)
		{
			SetOSProfile();
			this->RGBUpdated();
		}
		else if (this->rgb.monProfileType == Media::ColorProfile::CPT_EDID)
		{
			SetEDIDProfile();
			this->RGBUpdated();
		}
		else
		{
			this->rgb.monProfile.SetCommonProfile(newVal);
			this->RGBUpdated();
		}
	}
}

Bool Media::MonitorColorManager::SetMonProfileFile(NN<Text::String> fileName)
{
	if (SetFromProfileFile(fileName))
	{
		OPTSTR_DEL(this->monProfileFile);
		this->monProfileFile = fileName->Clone();
		this->rgb.monProfileType = Media::ColorProfile::CPT_FILE;
		this->RGBUpdated();
		return true;
	}
	else
	{
		return false;
	}
}

void Media::MonitorColorManager::SetMonProfile(NN<const Media::ColorProfile> color)
{
	this->rgb.monProfileType = Media::ColorProfile::CPT_CUSTOM;
	this->rgb.monProfile.Set(color);
	this->RGBUpdated();
}

Optional<Text::String> Media::MonitorColorManager::GetMonProfileFile()
{
	return this->monProfileFile;
}

void Media::MonitorColorManager::SetMonLuminance(Double newVal)
{
	if (newVal != this->rgb.monLuminance && newVal >= 0 && newVal >= 10.0)
	{
		this->rgb.monLuminance = newVal;
		this->RGBUpdated();
	}
}

Bool Media::MonitorColorManager::Get10BitColor()
{
	return this->color10Bit;
}

void Media::MonitorColorManager::Set10BitColor(Bool color10Bit)
{
	this->color10Bit = color10Bit;
}

void Media::MonitorColorManager::AddSess(NN<Media::ColorManagerSess> colorSess)
{
	Sync::MutexUsage mutUsage(this->sessMut);
	this->sessList.Add(colorSess);
}

void Media::MonitorColorManager::RemoveSess(NN<Media::ColorManagerSess> colorSess)
{
	UOSInt i;
	Sync::MutexUsage mutUsage(this->sessMut);
	i = this->sessList.IndexOf(colorSess);
	if (i != INVALID_INDEX)
	{
		this->sessList.RemoveAt((UOSInt)i);
	}
}

Bool Media::MonitorColorManager::SetFromProfileFile(NN<Text::String> fileName)
{
	Bool succ = false;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	UInt64 fileSize = fs.GetLength();
	if (fileSize > 0 && fileSize < 1048576)
	{
		NN<Media::ICCProfile> profile;
		Data::ByteBuffer fileBuff((UOSInt)fileSize);
		if (fs.Read(fileBuff) == fileSize)
		{
			if (Media::ICCProfile::Parse(fileBuff).SetTo(profile))
			{
				Media::ColorProfile cp;
				if (profile->GetColorPrimaries(cp.GetPrimaries()) && profile->GetRedTransferParam(cp.GetRTranParam()) && profile->GetGreenTransferParam(cp.GetGTranParam()) && profile->GetBlueTransferParam(cp.GetBTranParam()))
				{
					succ = true;
					this->rgb.monProfile.Set(cp);
					this->rgb.monProfile.SetRAWICC(fileBuff.GetPtr());
				}
				profile.Delete();
			}
		}
	}
	return succ;
}

void Media::MonitorColorManager::SetOSProfile()
{
#if (defined(_WIN32) || defined(_WIN64)) && !defined(_WIN32_WCE)
	WChar wbuff[512];
	WChar wbuff2[512];
	Bool succ = false;
	DISPLAY_DEVICEW dev;
	NN<Text::String> s;
	UInt32 i = 0;
	UOSInt j;
	dev.cb = sizeof(DISPLAY_DEVICEW);
	if (this->profileName.SetTo(s))
	{
		const WChar *wprofileName = Text::StrToWCharNew(s->v);
		while (EnumDisplayDevicesW(0, i, &dev, 0) != 0)
		{
			Text::StrConcat(wbuff2, dev.DeviceName);
			if (EnumDisplayDevicesW(wbuff2, 0, &dev, 0) != 0)
			{
				j = Text::StrIndexOfChar(dev.DeviceID, '\\');
				if (Text::StrStartsWith(&dev.DeviceID[j + 1], wprofileName))
				{
					HDC hdc = CreateDCW(L"DISPLAY", wbuff2, 0, 0);
					if (hdc)
					{
						DWORD size;
						size = 512;
						if (GetICMProfileW(hdc, &size, wbuff) != 0)
						{
							NN<Text::String> s = Text::String::NewNotNull(wbuff);
							succ = SetFromProfileFile(s);
							s->Release();
						}
						DeleteDC(hdc);
					}

					break;
				}
			}
			i++;
		}
		Text::StrDelNew(wprofileName);
	}
	if (!succ)
	{
		this->rgb.monProfile.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
	}
#else
	this->rgb.monProfile.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
#endif
}

void Media::MonitorColorManager::SetEDIDProfile()
{
	Bool succ = false;
	NN<Text::String> s;
	if (this->profileName.SetTo(s))
	{
		Media::DDCReader ddc(s->v);
		UOSInt edidSize;
		UInt8 *edid = ddc.GetEDID(edidSize);
		if (edid)
		{
			Media::EDID::EDIDInfo info;
			if (Media::EDID::Parse(edid, info))
			{
				if (Media::EDID::SetColorProfile(info, this->rgb.monProfile))
				{
					succ = true;
				}
			}
		}
	}
	if (!succ)
	{
		this->SetOSProfile();
	}
}

void Media::MonitorColorManager::RGBUpdated()
{
	Sync::MutexUsage mutUsage(this->sessMut);
	Data::ArrayIterator<NN<Media::ColorManagerSess>> it = this->sessList.Iterator();
	while (it.HasNext())
	{
		it.Next()->RGBUpdated(this->rgb);
	}
}

void Media::MonitorColorManager::YUVUpdated()
{
	Sync::MutexUsage mutUsage(this->sessMut);
	Data::ArrayIterator<NN<Media::ColorManagerSess>> it = this->sessList.Iterator();
	while (it.HasNext())
	{
		it.Next()->YUVUpdated(this->yuv);
	}
}

Media::ColorManager::ColorManager()
{
	this->defVProfileType = Media::ColorProfile::CPT_BT709;
	this->defVProfile.SetCommonProfile(this->defVProfileType);
	this->defPProfileType = Media::ColorProfile::CPT_SRGB;
	this->defPProfile.SetCommonProfile(this->defPProfileType);
	this->LoadDef();
}

Media::ColorManager::~ColorManager()
{
	NN<MonitorColorManager> monColor;
	UOSInt i = this->monColor.GetCount();
	while (i-- > 0)
	{
		monColor = this->monColor.GetItemNoCheck(i);
		monColor.Delete();
	}
}

Bool Media::ColorManager::LoadDef()
{
	IO::Registry *regBase;
	IO::Registry *reg;

	regBase = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"Color");
	if (regBase)
	{
		reg = regBase;

		Int32 tmpVal;

		if (reg->GetValueI32(L"DefVProfileType", tmpVal))
		{
			this->defVProfileType = (Media::ColorProfile::CommonProfileType)tmpVal;
			this->defVProfile.SetCommonProfile(this->defVProfileType);
		}
		if (reg->GetValueI32(L"DefPProfileType", tmpVal))
		{
			this->defPProfileType = (Media::ColorProfile::CommonProfileType)tmpVal;
			this->defPProfile.SetCommonProfile(this->defPProfileType);
		}
		if (reg->GetValueI32(L"YUVType", tmpVal))
			this->defYUVType = (Media::ColorProfile::YUVType)tmpVal;

		IO::Registry::CloseRegistry(regBase);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::ColorManager::SaveDef()
{
	IO::Registry *regBase;
	IO::Registry *reg;

	regBase = IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"Color");
	if (regBase)
	{
		reg = regBase;

		reg->SetValue(L"DefVProfileType", (Int32)this->defVProfileType);
		reg->SetValue(L"DefPProfileType", (Int32)this->defPProfileType);
		reg->SetValue(L"YUVType", (Int32)this->defYUVType);

		IO::Registry::CloseRegistry(regBase);
		return true;
	}
	else
	{
		return false;
	}
}

void Media::ColorManager::SetDefVProfile(Media::ColorProfile::CommonProfileType newVal)
{
	if (newVal != this->defVProfileType)
	{
		this->defVProfileType = newVal;
		this->defVProfile.SetCommonProfile(newVal);
	}
}

void Media::ColorManager::SetDefPProfile(Media::ColorProfile::CommonProfileType newVal)
{
	if (newVal != this->defPProfileType)
	{
		this->defPProfileType = newVal;
		this->defPProfile.SetCommonProfile(newVal);
	}
}

NN<Media::ColorProfile> Media::ColorManager::GetDefVProfile()
{
	return this->defVProfile;
}

NN<Media::ColorProfile> Media::ColorManager::GetDefPProfile()
{
	return this->defPProfile;
}

Media::ColorProfile::CommonProfileType Media::ColorManager::GetDefVProfileType()
{
	return this->defVProfileType;
}

Media::ColorProfile::CommonProfileType Media::ColorManager::GetDefPProfileType()
{
	return this->defPProfileType;
}

void Media::ColorManager::SetYUVType(Media::ColorProfile::YUVType newVal)
{
	if (newVal != this->defYUVType)
	{
		this->defYUVType = newVal;
	}
}

Media::ColorProfile::YUVType Media::ColorManager::GetDefYUVType()
{
	return this->defYUVType;
}

NN<Media::MonitorColorManager> Media::ColorManager::GetMonColorManager(Text::String *profileName)
{
	Optional<Media::MonitorColorManager> monColor;
	NN<Media::MonitorColorManager> nnmonColor;
	Sync::MutexUsage mutUsage(this->mut);
	if (profileName == 0)
	{
		monColor = this->monColor.GetC(CSTR(""));
	}
	else
	{
		monColor = this->monColor.Get(profileName);
	}
	if (!monColor.SetTo(nnmonColor))
	{
		NEW_CLASSNN(nnmonColor, Media::MonitorColorManager(profileName));
		if (profileName == 0)
		{
			this->monColor.PutNN(Text::String::NewEmpty(), nnmonColor);
		}
		else
		{
			this->monColor.Put(profileName, nnmonColor);
		}
		return nnmonColor;
	}
	else
	{
		return nnmonColor;
	}
}

NN<Media::MonitorColorManager> Media::ColorManager::GetMonColorManager(MonitorHandle *hMon)
{
	Media::MonitorInfo monInfo(hMon);
	return GetMonColorManager(monInfo.GetMonitorID());
}

NN<Media::ColorManagerSess> Media::ColorManager::CreateSess(MonitorHandle *hMon)
{
	NN<Media::MonitorColorManager> monColor;
	monColor = this->GetMonColorManager(hMon);
	NN<Media::ColorManagerSess> colorSess;
	NEW_CLASSNN(colorSess, Media::ColorManagerSess(*this, monColor));
	return colorSess;
}

void Media::ColorManager::DeleteSess(NN<ColorManagerSess> sess)
{
	sess.Delete();
}

Media::ColorManagerSess::ColorManagerSess(NN<ColorManager> colorMgr, NN<MonitorColorManager> monColor)
{
	this->colorMgr = colorMgr;
	this->monColor = monColor;
	this->monColor->AddSess(*this);
}

Media::ColorManagerSess::~ColorManagerSess()
{
	this->monColor->RemoveSess(*this);
}

void Media::ColorManagerSess::AddHandler(NN<Media::IColorHandler> hdlr)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrs.Add(hdlr);
}

void Media::ColorManagerSess::RemoveHandler(NN<Media::IColorHandler> hdlr)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt index = this->hdlrs.IndexOf(hdlr);
	if (index != INVALID_INDEX)
	{
		this->hdlrs.RemoveAt((UOSInt)index);
	}
}

NN<const Media::IColorHandler::YUVPARAM> Media::ColorManagerSess::GetYUVParam()
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	return this->monColor->GetYUVParam();
}

NN<const Media::IColorHandler::RGBPARAM2> Media::ColorManagerSess::GetRGBParam()
{
	Sync::RWMutexUsage mutUsage(this->mut, false);
	return this->monColor->GetRGBParam();
}

NN<Media::ColorProfile> Media::ColorManagerSess::GetDefVProfile()
{
	return this->colorMgr->GetDefVProfile();
}

NN<Media::ColorProfile> Media::ColorManagerSess::GetDefPProfile()
{
	return this->colorMgr->GetDefPProfile();
}

Media::ColorProfile::YUVType Media::ColorManagerSess::GetDefYUVType()
{
	return this->colorMgr->GetDefYUVType();
}

Bool Media::ColorManagerSess::Get10BitColor()
{
	return this->monColor->Get10BitColor();
}

void Media::ColorManagerSess::ChangeMonitor(MonitorHandle *hMon)
{
	Media::MonitorInfo monInfo(hMon);
	Text::String *monName = monInfo.GetMonitorID();
	if (monName == 0)
		return;
	this->mut.LockWrite();
	if (Text::StringTool::Equals(monName, this->monColor->GetProfileName()))
	{
		this->mut.UnlockWrite();
		return;
	}
	this->monColor->RemoveSess(*this);
	this->monColor = this->colorMgr->GetMonColorManager(monName);
	this->monColor->AddSess(*this);
	this->mut.UnlockWrite();
	this->RGBUpdated(this->GetRGBParam());
	this->YUVUpdated(this->GetYUVParam());
}

void Media::ColorManagerSess::RGBUpdated(NN<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	Data::ArrayIterator<NN<Media::IColorHandler>> it = this->hdlrs.Iterator();
	while (it.HasNext())
	{
		it.Next()->RGBParamChanged(rgbParam);
	}
}

void Media::ColorManagerSess::YUVUpdated(NN<const Media::IColorHandler::YUVPARAM> yuvParam)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	Data::ArrayIterator<NN<Media::IColorHandler>> it = this->hdlrs.Iterator();
	while (it.HasNext())
	{
		it.Next()->YUVParamChanged(yuvParam);
	}
}
