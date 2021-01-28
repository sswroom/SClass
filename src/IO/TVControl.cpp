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

OSInt IO::TVControl::GetTVList(Data::ArrayList<TVType> *tvList)
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

IO::TVControl *IO::TVControl::CreateTVControl(IO::Stream *stm, TVType tvType, Int32 monId)
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
		return 0;
	}
}

const UTF8Char *IO::TVControl::GetCommandName(CommandType cmdType)
{
	switch (cmdType)
	{
	case CT_POWERON:
		return (const UTF8Char*)"Power On";
	case CT_POWEROFF:
		return (const UTF8Char*)"Power Off";
	case CT_FORCEPOWEROFF:
		return (const UTF8Char*)"Force Power Off";
	case CT_GETPOWERSTATUS:
		return (const UTF8Char*)"Get Power Status";
	case CT_AUTOSETUP:
		return (const UTF8Char*)"Auto Setup";
	case CT_FACTORYRESET:
		return (const UTF8Char*)"Factory Reset";
	case CT_SCREENRESET:
		return (const UTF8Char*)"Screen Reset";
	case CT_VOLUMEUP:
		return (const UTF8Char*)"Volume Up";
	case CT_VOLUMEDOWN:
		return (const UTF8Char*)"Volume Down";
	case CT_AUDIOMUTE:
		return (const UTF8Char*)"Audio Mute";
	case CT_INPUT_DSUB:
		return (const UTF8Char*)"Input - D-Sub";
	case CT_INPUT_VIDEO:
		return (const UTF8Char*)"Input - Video";
	case CT_INPUT_HDMI1:
		return (const UTF8Char*)"Input - HDMI1";
	case CT_INPUT_HDMI2:
		return (const UTF8Char*)"Input - HDMI2";
	case CT_INPUT_DVI_D:
		return (const UTF8Char*)"Input - DVI-D";
	case CT_INPUT_DISPLAYPORT:
		return (const UTF8Char*)"Input - DisplayPort";
	case CT_INPUT_DISPLAYPORT2:
		return (const UTF8Char*)"Input - DisplayPort2";
	case CT_INPUT_DISPLAYPORT3:
		return (const UTF8Char*)"Input - DisplayPort3";
	case CT_INPUT_YPBPR:
		return (const UTF8Char*)"Input - Component";
	case CT_INPUT_YPBPR2:
		return (const UTF8Char*)"Input - Component2";
	case CT_INPUT_SVIDEO:
		return (const UTF8Char*)"Input - S-Video";
	case CT_GETCURRINPUT:
		return (const UTF8Char*)"Get Video Input";
	case CT_GETDATETIME:
		return (const UTF8Char*)"Get Date Time";
	case CT_SETCURRDATETIME:
		return (const UTF8Char*)"Set Current Date Time";
	case CT_SELFDIAGNOSIS:
		return (const UTF8Char*)"Self Diagnosis";
	case CT_GETSERIALNO:
		return (const UTF8Char*)"Get Serial Number";
	case CT_GETMODEL:
		return (const UTF8Char*)"Get Model";
	case CT_GETFIRMWARE:
		return (const UTF8Char*)"Get Firmware Version";
	case CT_GETFIRMWARE2:
		return (const UTF8Char*)"Get Firmware2 Version";
	case CT_GETPOWERSAVEMODE:
		return (const UTF8Char*)"Get Power Save Mode";
	case CT_GETPOWERSAVETIME:
		return (const UTF8Char*)"Get Power Save Time";
	case CT_GETAUTOSTANDBYTIME:
		return (const UTF8Char*)"Get Auto Standby Time";
	case CT_GETLANMAC:
		return (const UTF8Char*)"Get LAN MAC Address";
	case CT_GETLANIP:
		return (const UTF8Char*)"Get LAN IP Address";
	case CT_GETTEMPERATURE1:
		return (const UTF8Char*)"Get Temperature1";
	case CT_GETTEMPERATURE2:
		return (const UTF8Char*)"Get Temperature2";
	case CT_GETTEMPERATURE3:
		return (const UTF8Char*)"Get Temperature3";
	case CT_GETBACKLIGHT:
		return (const UTF8Char*)"Get Backlight";
	case CT_SETBACKLIGHT:
		return (const UTF8Char*)"Set Backlight";
	case CT_GETCONTRAST:
		return (const UTF8Char*)"Get Contrast";
	case CT_SETCONTRAST:
		return (const UTF8Char*)"Set Contrast";
	case CT_GETSHARPNESS:
		return (const UTF8Char*)"Get Sharpness";
	case CT_SETSHARPNESS:
		return (const UTF8Char*)"Set Sharpness";
	case CT_GETBRIGHTNESS:
		return (const UTF8Char*)"Get Brightness";
	case CT_SETBRIGNTNESS:
		return (const UTF8Char*)"Set Brightness";
	case CT_GETHUE:
		return (const UTF8Char*)"Get Hue";
	case CT_SETHUE:
		return (const UTF8Char*)"Set Hue";
	case CT_GETSATURATION:
		return (const UTF8Char*)"Get Saturation";
	case CT_SETSATURATION:
		return (const UTF8Char*)"Set Saturation";
	case CT_GETCOLORTEMP:
		return (const UTF8Char*)"Get Color Temperature";
	case CT_SETCOLORTEMP:
		return (const UTF8Char*)"Set Color Temperature";
	case CT_IRLOCK:
		return (const UTF8Char*)"IR Lock";
	case CT_IRUNLOCK:
		return (const UTF8Char*)"IR Unlock";
	case CT_GETIRSTATUS:
		return (const UTF8Char*)"Get IR Status";
	case CT_GETHPOS:
		return (const UTF8Char*)"Get H-Pos";
	case CT_GETVPOS:
		return (const UTF8Char*)"Get V-Pos";
	default:
		return (const UTF8Char*)"";
	}
}
