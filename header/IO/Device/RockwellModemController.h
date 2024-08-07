#ifndef _SM_IO_DEVICE_ROCKWELLMODEMCONTROLLER
#define _SM_IO_DEVICE_ROCKWELLMODEMCONTROLLER
#include "IO/ModemController.h"

namespace IO
{
	namespace Device
	{
		class RockwellModemController : public IO::ModemController
		{
		public:
			typedef enum
			{
				VT_DATA = 0,
				VT_CLASS1FAX = 1,
				VT_CLASS2FAX = 2,
				VT_VOICE = 8
			} VoiceType;

			typedef enum
			{
				CIDT_DISABLE = 0,
				CIDT_FORMATED = 1,
				CIDT_UNFORMATED = 2
			} CallerIDType;

			typedef enum
			{
				VLT_PHONE_LINE = 0,
				VLT_HANDSET = 1,
				VLT_SPEAKER = 2,
				VLT_MICROPHONE = 3,
				VLT_INT_SPK = 4,
				VLT_EMULATION = 5,
				VLT_SPEAKERPHONE = 6,
				VLT_MUTE_HANDSET = 7,
				VLT_CALLER_ID_RELAY = 8,
				VLT_CODEC = 9
			} VoiceLineType;

		public:
			RockwellModemController(NN<IO::ATCommandChannel> channel, Bool needRelease);
			virtual ~RockwellModemController();

			// Voice Commands
			UnsafeArrayOpt<UTF8Char> VoiceGetManufacturer(UnsafeArray<UTF8Char> manu);
			UnsafeArrayOpt<UTF8Char> VoiceGetModel(UnsafeArray<UTF8Char> model);
			UnsafeArrayOpt<UTF8Char> VoiceGetRevision(UnsafeArray<UTF8Char> ver);
			Bool VoiceGetBaudRate(OutParam<Int32> baudRate);
			Bool VoiceSetBaudRate(Int32 baudRate);
			Bool VoiceGetCallerIDType(OutParam<CallerIDType> callerIDType);
			Bool VoiceSetCallerIDType(CallerIDType callerIDType);
			Bool VoiceGetType(OutParam<VoiceType> voiceType);
			Bool VoiceSetType(VoiceType voiceType);
			Bool VoiceGetBufferSize(OutParam<Int32> buffSize);
			Bool VoiceGetBitsPerSample(OutParam<Int32> bps);
			Bool VoiceSetBitsPerSample(Int32 bps);
			Bool VoiceGetToneDur(OutParam<Int32> durMS);
			Bool VoiceSetToneDur(Int32 durMS);
			UnsafeArrayOpt<UTF8Char> VoiceGetCompression(UnsafeArray<UTF8Char> comp);
			Bool VoiceGetVoiceLineType(OutParam<VoiceLineType> voiceLineType);
			Bool VoiceSetVoiceLineType(VoiceLineType voiceLineType);
			DialResult VoiceToneDial(UnsafeArray<const UTF8Char> phoneNum);
			DialResult VoicePulseDial(UnsafeArray<const UTF8Char> phoneNum);

		public:
			static UnsafeArray<UTF8Char> GetVoiceTypeString(UnsafeArray<UTF8Char> buff, VoiceType voiceType);
			static UnsafeArray<UTF8Char> GetCallerIDTypeString(UnsafeArray<UTF8Char> buff, CallerIDType callerIDType);
			static UnsafeArray<UTF8Char> GetVoiceLineTypeString(UnsafeArray<UTF8Char> buff, VoiceLineType voiceLineType);
		};
	}
}
#endif
