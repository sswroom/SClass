#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/RockwellModemController.h"
#include "Text/MyString.h"

IO::Device::RockwellModemController::RockwellModemController(IO::ATCommandChannel *channel, Bool needRelease) : IO::ModemController(channel, needRelease)
{
}

IO::Device::RockwellModemController::~RockwellModemController()
{
}

UTF8Char *IO::Device::RockwellModemController::VoiceGetManufacturer(UTF8Char *manu)
{
	return this->SendStringCommand(manu, UTF8STRC("AT#MFR?"), 3000);
}

UTF8Char *IO::Device::RockwellModemController::VoiceGetModel(UTF8Char *model)
{
	return this->SendStringCommand(model, UTF8STRC("AT#MDL?"), 3000);
}

UTF8Char *IO::Device::RockwellModemController::VoiceGetRevision(UTF8Char *ver)
{
	return this->SendStringCommand(ver, UTF8STRC("AT#REV?"), 3000);
}

Bool IO::Device::RockwellModemController::VoiceGetBaudRate(Int32 *baudRate)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#BDR?"), 3000) == 0)
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, &v))
		return false;
	*baudRate = v * 2400;
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetBaudRate(Int32 baudRate)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#BDR=")), baudRate / 2400);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::RockwellModemController::VoiceGetCallerIDType(CallerIDType *callerIDType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#CID?"), 3000) == 0)
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, &v))
		return false;
	*callerIDType = (CallerIDType)v;
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetCallerIDType(CallerIDType callerIDType)
{
	switch (callerIDType)
	{
	case CIDT_DISABLE:
		return this->SendBoolCommandC(UTF8STRC("AT#CID=0"));
	case CIDT_FORMATED:
		return this->SendBoolCommandC(UTF8STRC("AT#CID=1"));
	case CIDT_UNFORMATED:
		return this->SendBoolCommandC(UTF8STRC("AT#CID=2"));
	default:
		return false;
	}
}

Bool IO::Device::RockwellModemController::VoiceGetType(VoiceType *voiceType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#CLS?"), 3000) == 0)
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, &v))
		return false;
	*voiceType = (VoiceType)v;
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetType(VoiceType voiceType)
{
	switch (voiceType)
	{
	case VT_DATA:
		return this->SendBoolCommandC(UTF8STRC("AT#CLS=0"));
	case VT_CLASS1FAX:
		return this->SendBoolCommandC(UTF8STRC("AT#CLS=1"));
	case VT_CLASS2FAX:
		return this->SendBoolCommandC(UTF8STRC("AT#CLS=2"));
	case VT_VOICE:
		return this->SendBoolCommandC(UTF8STRC("AT#CLS=8"));
	default:
		return false;
	}
}

Bool IO::Device::RockwellModemController::VoiceGetBufferSize(Int32 *buffSize)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBQ?"), 3000) == 0)
		return false;
	return Text::StrToInt32(sbuff, buffSize);
}

Bool IO::Device::RockwellModemController::VoiceGetBitsPerSample(Int32 *bps)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBS?"), 3000) == 0)
		return false;
	return Text::StrToInt32(sbuff, bps);
}

Bool IO::Device::RockwellModemController::VoiceSetBitsPerSample(Int32 bps)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VBS=")), bps);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::Device::RockwellModemController::VoiceGetToneDur(Int32 *durMS)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBS?"), 3000) == 0)
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, &v))
		return false;
	*durMS = v * 100;
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetToneDur(Int32 durMS)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VBT=")), durMS / 100);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

UTF8Char *IO::Device::RockwellModemController::VoiceGetCompression(UTF8Char *comp)
{
	return this->SendStringCommand(comp, UTF8STRC("AT#VCI?"), 3000);
}

Bool IO::Device::RockwellModemController::VoiceGetVoiceLineType(VoiceLineType *voiceLineType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VLS?"), 3000) == 0)
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, &v))
		return false;
	*voiceLineType = (VoiceLineType)v;
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetVoiceLineType(VoiceLineType voiceLineType)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VLS=")), (Int32)voiceLineType);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

IO::ModemController::DialResult IO::Device::RockwellModemController::VoiceToneDial(const UTF8Char *phoneNum)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("ATDT")), phoneNum);
	return this->SendDialCommand(sbuff, (UOSInt)(sptr - sbuff));
}

IO::ModemController::DialResult IO::Device::RockwellModemController::VoicePulseDial(const UTF8Char *phoneNum)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("ATDP")), phoneNum);
	return this->SendDialCommand(sbuff, (UOSInt)(sptr - sbuff));
}

UTF8Char *IO::Device::RockwellModemController::GetVoiceTypeString(UTF8Char *buff, VoiceType voiceType)
{
	switch (voiceType)
	{
	case VT_DATA:
		return Text::StrConcatC(buff, UTF8STRC("Data Mode"));
	case VT_CLASS1FAX:
		return Text::StrConcatC(buff, UTF8STRC("Class 1 fax"));
	case VT_CLASS2FAX:
		return Text::StrConcatC(buff, UTF8STRC("Class 2 fax"));
	case VT_VOICE:
		return Text::StrConcatC(buff, UTF8STRC("Voice/Audio Mode"));
	default:
		return Text::StrConcatC(buff, UTF8STRC("Unknown"));
	}
}

UTF8Char *IO::Device::RockwellModemController::GetCallerIDTypeString(UTF8Char *buff, CallerIDType callerIDType)
{
	switch (callerIDType)
	{
	case CIDT_DISABLE:
		return Text::StrConcatC(buff, UTF8STRC("Disable Caller ID"));
	case CIDT_FORMATED:
		return Text::StrConcatC(buff, UTF8STRC("Enable Caller ID with formatted data"));
	case CIDT_UNFORMATED:
		return Text::StrConcatC(buff, UTF8STRC("Enable Caller ID with unformatted data"));
	default:
		return Text::StrConcatC(buff, UTF8STRC("Unknown"));
	}
}

UTF8Char *IO::Device::RockwellModemController::GetVoiceLineTypeString(UTF8Char *buff, VoiceLineType voiceLineType)
{
	switch (voiceLineType)
	{
	case VLT_PHONE_LINE:
		return Text::StrConcatC(buff, UTF8STRC("Phone Line"));
	case VLT_HANDSET:
		return Text::StrConcatC(buff, UTF8STRC("Handset"));
	case VLT_SPEAKER:
		return Text::StrConcatC(buff, UTF8STRC("Speaker"));
	case VLT_MICROPHONE:
		return Text::StrConcatC(buff, UTF8STRC("Auxiliary Input device (Microphone)"));
	case VLT_INT_SPK:
		return Text::StrConcatC(buff, UTF8STRC("Phone Line with internal speaker"));
	case VLT_EMULATION:
		return Text::StrConcatC(buff, UTF8STRC("Telephone Emulation Mode"));
	case VLT_SPEAKERPHONE:
		return Text::StrConcatC(buff, UTF8STRC("Speakerphone Mode"));
	case VLT_MUTE_HANDSET:
		return Text::StrConcatC(buff, UTF8STRC("Mutes local handset"));
	case VLT_CALLER_ID_RELAY:
		return Text::StrConcatC(buff, UTF8STRC("Caller ID relay"));
	case VLT_CODEC:
		return Text::StrConcatC(buff, UTF8STRC("Sound Codec"));
	default:
		return Text::StrConcatC(buff, UTF8STRC("Unknown"));
	}
}
