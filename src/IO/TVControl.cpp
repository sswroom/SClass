#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"
#include "IO/TVControl.h"
#include "IO/TVCtrl/MDT701STVControl.h"
#include "IO/TVCtrl/NECTVControl.h"

IO::TVControl::TVControl()
{
}

IO::TVControl::~TVControl()
{
}

IntOS IO::TVControl::GetTVList(Data::ArrayListNative<TVType> *tvList)
{
	tvList->Add(TVT_MDT701S);
	tvList->Add(TVT_NEC);
	return 1;
}

Bool IO::TVControl::GetTVInfo(TVType tvType, TVInfo *info)
{
	switch (tvType)
	{
	case TVT_MDT701S:
		return IO::TVCtrl::MDT701STVControl::GetInfo(info);
	case TVT_NEC:
		return IO::TVCtrl::NECTVControl::GetInfo(info);
	default:
		return false;
	}
}

Optional<IO::TVControl> IO::TVControl::CreateTVControl(NN<IO::Stream> stm, TVType tvType, Int32 monId)
{
	IO::TVControl *tvCtrl;
	switch (tvType)
	{
	case TVT_MDT701S:
		NEW_CLASS(tvCtrl, IO::TVCtrl::MDT701STVControl(stm, monId));
		return tvCtrl;
	case TVT_NEC:
		NEW_CLASS(tvCtrl, IO::TVCtrl::NECTVControl(stm, monId));
		return tvCtrl;
	default:
		return nullptr;
	}
}

Text::CStringNN IO::TVControl::GetCommandName(CommandType cmdType)
{
	switch (cmdType)
	{
	case CT_POWERON:
		return CSTR("Power On");
	case CT_POWEROFF:
		return CSTR("Power Off");
	case CT_FORCEPOWEROFF:
		return CSTR("Force Power Off");
	case CT_GETPOWERSTATUS:
		return CSTR("Get Power Status");
	case CT_AUTOSETUP:
		return CSTR("Auto Setup");
	case CT_FACTORYRESET:
		return CSTR("Factory Reset");
	case CT_SCREENRESET:
		return CSTR("Screen Reset");
	case CT_VOLUMEUP:
		return CSTR("Volume Up");
	case CT_VOLUMEDOWN:
		return CSTR("Volume Down");
	case CT_AUDIOMUTE:
		return CSTR("Audio Mute");
	case CT_INPUT_DSUB:
		return CSTR("Input - D-Sub");
	case CT_INPUT_VIDEO:
		return CSTR("Input - Video");
	case CT_INPUT_HDMI1:
		return CSTR("Input - HDMI1");
	case CT_INPUT_HDMI2:
		return CSTR("Input - HDMI2");
	case CT_INPUT_DVI_D:
		return CSTR("Input - DVI-D");
	case CT_INPUT_DISPLAYPORT:
		return CSTR("Input - DisplayPort");
	case CT_INPUT_DISPLAYPORT2:
		return CSTR("Input - DisplayPort2");
	case CT_INPUT_DISPLAYPORT3:
		return CSTR("Input - DisplayPort3");
	case CT_INPUT_YPBPR:
		return CSTR("Input - Component");
	case CT_INPUT_YPBPR2:
		return CSTR("Input - Component2");
	case CT_INPUT_SVIDEO:
		return CSTR("Input - S-Video");
	case CT_GETCURRINPUT:
		return CSTR("Get Video Input");
	case CT_GETDATETIME:
		return CSTR("Get Date Time");
	case CT_SETCURRDATETIME:
		return CSTR("Set Current Date Time");
	case CT_SELFDIAGNOSIS:
		return CSTR("Self Diagnosis");
	case CT_GETSERIALNO:
		return CSTR("Get Serial Number");
	case CT_GETMODEL:
		return CSTR("Get Model");
	case CT_GETFIRMWARE:
		return CSTR("Get Firmware Version");
	case CT_GETFIRMWARE2:
		return CSTR("Get Firmware2 Version");
	case CT_GETPOWERSAVEMODE:
		return CSTR("Get Power Save Mode");
	case CT_GETPOWERSAVETIME:
		return CSTR("Get Power Save Time");
	case CT_GETAUTOSTANDBYTIME:
		return CSTR("Get Auto Standby Time");
	case CT_GETLANMAC:
		return CSTR("Get LAN MAC Address");
	case CT_GETLANIP:
		return CSTR("Get LAN IP Address");
	case CT_GETTEMPERATURE1:
		return CSTR("Get Temperature1");
	case CT_GETTEMPERATURE2:
		return CSTR("Get Temperature2");
	case CT_GETTEMPERATURE3:
		return CSTR("Get Temperature3");
	case CT_GETBACKLIGHT:
		return CSTR("Get Backlight");
	case CT_SETBACKLIGHT:
		return CSTR("Set Backlight");
	case CT_GETCONTRAST:
		return CSTR("Get Contrast");
	case CT_SETCONTRAST:
		return CSTR("Set Contrast");
	case CT_GETSHARPNESS:
		return CSTR("Get Sharpness");
	case CT_SETSHARPNESS:
		return CSTR("Set Sharpness");
	case CT_GETBRIGHTNESS:
		return CSTR("Get Brightness");
	case CT_SETBRIGNTNESS:
		return CSTR("Set Brightness");
	case CT_GETHUE:
		return CSTR("Get Hue");
	case CT_SETHUE:
		return CSTR("Set Hue");
	case CT_GETSATURATION:
		return CSTR("Get Saturation");
	case CT_SETSATURATION:
		return CSTR("Set Saturation");
	case CT_GETCOLORTEMP:
		return CSTR("Get Color Temperature");
	case CT_SETCOLORTEMP:
		return CSTR("Set Color Temperature");
	case CT_IRLOCK:
		return CSTR("IR Lock");
	case CT_IRUNLOCK:
		return CSTR("IR Unlock");
	case CT_GETIRSTATUS:
		return CSTR("Get IR Status");
	case CT_GETHPOS:
		return CSTR("Get H-Pos");
	case CT_GETVPOS:
		return CSTR("Get V-Pos");
	default:
		return CSTR("");
	}
}
