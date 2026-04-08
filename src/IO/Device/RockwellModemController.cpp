#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/RockwellModemController.h"
#include "Text/MyString.h"

IO::Device::RockwellModemController::RockwellModemController(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::ModemController(channel, needRelease)
{
}

IO::Device::RockwellModemController::~RockwellModemController()
{
}

UnsafeArrayOpt<UTF8Char> IO::Device::RockwellModemController::VoiceGetManufacturer(UnsafeArray<UTF8Char> manu)
{
	return this->SendStringCommand(manu, UTF8STRC("AT#MFR?"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::Device::RockwellModemController::VoiceGetModel(UnsafeArray<UTF8Char> model)
{
	return this->SendStringCommand(model, UTF8STRC("AT#MDL?"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::Device::RockwellModemController::VoiceGetRevision(UnsafeArray<UTF8Char> ver)
{
	return this->SendStringCommand(ver, UTF8STRC("AT#REV?"), 3000);
}

Bool IO::Device::RockwellModemController::VoiceGetBaudRate(OutParam<Int32> baudRate)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#BDR?"), 3000).IsNull())
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, v))
		return false;
	baudRate.Set(v * 2400);
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetBaudRate(Int32 baudRate)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#BDR=")), baudRate / 2400);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

Bool IO::Device::RockwellModemController::VoiceGetCallerIDType(OutParam<CallerIDType> callerIDType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#CID?"), 3000).IsNull())
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, v))
		return false;
	callerIDType.Set((CallerIDType)v);
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

Bool IO::Device::RockwellModemController::VoiceGetType(OutParam<VoiceType> voiceType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#CLS?"), 3000).IsNull())
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, v))
		return false;
	voiceType.Set((VoiceType)v);
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

Bool IO::Device::RockwellModemController::VoiceGetBufferSize(OutParam<Int32> buffSize)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBQ?"), 3000).IsNull())
		return false;
	return Text::StrToInt32(sbuff, buffSize);
}

Bool IO::Device::RockwellModemController::VoiceGetBitsPerSample(OutParam<Int32> bps)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBS?"), 3000).IsNull())
		return false;
	return Text::StrToInt32(sbuff, bps);
}

Bool IO::Device::RockwellModemController::VoiceSetBitsPerSample(Int32 bps)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VBS=")), bps);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

Bool IO::Device::RockwellModemController::VoiceGetToneDur(OutParam<Int32> durMS)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VBS?"), 3000).IsNull())
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, v))
		return false;
	durMS.Set(v * 100);
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetToneDur(Int32 durMS)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VBT=")), durMS / 100);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

UnsafeArrayOpt<UTF8Char> IO::Device::RockwellModemController::VoiceGetCompression(UnsafeArray<UTF8Char> comp)
{
	return this->SendStringCommand(comp, UTF8STRC("AT#VCI?"), 3000);
}

Bool IO::Device::RockwellModemController::VoiceGetVoiceLineType(OutParam<VoiceLineType> voiceLineType)
{
	UTF8Char sbuff[128];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT#VLS?"), 3000).IsNull())
		return false;
	Int32 v;
	if (!Text::StrToInt32(sbuff, v))
		return false;
	voiceLineType.Set((VoiceLineType)v);
	return true;
}

Bool IO::Device::RockwellModemController::VoiceSetVoiceLineType(VoiceLineType voiceLineType)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT#VLS=")), (Int32)voiceLineType);
	return this->SendBoolCommandC(sbuff, (UIntOS)(sptr - sbuff));
}

IO::ModemController::DialResult IO::Device::RockwellModemController::VoiceToneDial(UnsafeArray<const UTF8Char> phoneNum)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("ATDT")), phoneNum);
	return this->SendDialCommand(sbuff, (UIntOS)(sptr - sbuff));
}

IO::ModemController::DialResult IO::Device::RockwellModemController::VoicePulseDial(UnsafeArray<const UTF8Char> phoneNum)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("ATDP")), phoneNum);
	return this->SendDialCommand(sbuff, (UIntOS)(sptr - sbuff));
}

Text::CStringNN IO::Device::RockwellModemController::VoiceTypeGetName(VoiceType voiceType)
{
	switch (voiceType)
	{
	case VT_DATA:
		return CSTR("Data Mode");
	case VT_CLASS1FAX:
		return CSTR("Class 1 fax");
	case VT_CLASS2FAX:
		return CSTR("Class 2 fax");
	case VT_VOICE:
		return CSTR("Voice/Audio Mode");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::Device::RockwellModemController::CallerIDTypeGetName(CallerIDType callerIDType)
{
	switch (callerIDType)
	{
	case CIDT_DISABLE:
		return CSTR("Disable Caller ID");
	case CIDT_FORMATED:
		return CSTR("Enable Caller ID with formatted data");
	case CIDT_UNFORMATED:
		return CSTR("Enable Caller ID with unformatted data");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::Device::RockwellModemController::VoiceLineTypeGetName(VoiceLineType voiceLineType)
{
	switch (voiceLineType)
	{
	case VLT_PHONE_LINE:
		return CSTR("Phone Line");
	case VLT_HANDSET:
		return CSTR("Handset");
	case VLT_SPEAKER:
		return CSTR("Speaker");
	case VLT_MICROPHONE:
		return CSTR("Auxiliary Input device (Microphone)");
	case VLT_INT_SPK:
		return CSTR("Phone Line with internal speaker");
	case VLT_EMULATION:
		return CSTR("Telephone Emulation Mode");
	case VLT_SPEAKERPHONE:
		return CSTR("Speakerphone Mode");
	case VLT_MUTE_HANDSET:
		return CSTR("Mutes local handset");
	case VLT_CALLER_ID_RELAY:
		return CSTR("Caller ID relay");
	case VLT_CODEC:
		return CSTR("Sound Codec");
	default:
		return CSTR("Unknown");
	}
}
