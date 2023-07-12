#ifndef _SM_IO_CODESCANNER
#define _SM_IO_CODESCANNER
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class CodeScanner
	{
	public:
		typedef enum
		{
			MT_IDLE,
			MT_SCAN,
			MT_SETTING
		} ModeType;

		typedef enum
		{
			CT_UNKNOWN,
			CT_GET_COMMAND,
			CT_SET_COMMAND,
			CT_GET_COMMAND_NAME,
			CT_SELECT_COMMAND
		} CommandType;

		typedef enum
		{
			DC_GET_READ_MODE,
			DC_SET_READ_MODE,
			DC_GET_BW_MODE,
			DC_SET_BW_MODE,
			DC_GET_READ_REPEAT_TIME,
			DC_SET_READ_REPEAT_TIME,
			DC_GET_BRIGHTNESS,
			DC_SET_BRIGHTNESS,
			DC_GET_SHT_SIGNAL,
			DC_SET_SHT_SIGNAL,
			DC_GET_SCAN_MODE,
			DC_SET_SCAN_MODE,
			DC_GET_SHUTTER_TIME,
			DC_SET_SHUTTER_TIME,
			DC_GET_GAIN,
			DC_SET_GAIN,
			DC_GET_LED_LEVEL,
			DC_SET_LED_LEVEL,
			DC_GET_LED_MODE,
			DC_SET_LED_MODE,
			DC_GET_OUTPUT_TIMING,
			DC_SET_OUTPUT_TIMING,
			DC_GET_UNREAD_DATA_SEND,
			DC_SET_UNREAD_DATA_SEND,
			DC_GET_INDIR_TIME,
			DC_SET_INDIR_TIME,
			DC_GET_TRIGGER_DELAY,
			DC_SET_TRIGGER_DELAY,
			DC_GET_SIGNAL_ON_DUR,
			DC_SET_SIGNAL_ON_DUR,
			DC_GET_SIGNAL_DELAY,
			DC_SET_SIGNAL_DELAY,
			DC_GET_LIGHT_LED,
			DC_SET_LIGHT_LED,
			DC_GET_MARKER_LIGHT,
			DC_SET_MARKER_LIGHT,
			DC_GET_DECODE_TIME_LIMIT,
			DC_SET_DECODE_TIME_LIMIT,
			DC_GET_OUTPUT1_TYPE,
			DC_SET_OUTPUT1_TYPE,
			DC_GET_OUTPUT2_TYPE,
			DC_SET_OUTPUT2_TYPE,
			DC_GET_AUTO_SENSE_MODE,
			DC_SET_AUTO_SENSE_MODE,
			DC_GET_CONT_READ_MODE_B,
			DC_SET_CONT_READ_MODE_B,
			DC_GET_QRCODE,
			DC_SET_QRCODE,
			DC_GET_MICRO_QRCODE,
			DC_SET_MICRO_QRCODE,
			DC_GET_PDF417,
			DC_SET_PDF417,
			DC_GET_DATAMATRIX,
			DC_SET_DATAMATRIX,
			DC_GET_BARCODE,
			DC_SET_BARCODE,
			DC_GET_INTERLEAVED_2OF5,
			DC_SET_INTERLEAVED_2OF5,
			DC_GET_CODABAR,
			DC_SET_CODABAR,
			DC_GET_CODABAR_START_STOP,
			DC_SET_CODEBAR_START_STOP,
			DC_GET_CODE39,
			DC_SET_CODE39,
			DC_GET_CODE128,
			DC_SET_CODE128,
			DC_GET_QRCODE_REVERSE,
			DC_SET_QRCODE_REVERSE,
			DC_GET_QRLINK_CODE,
			DC_SET_QRLINK_CODE,
			DC_GET_GS1_DATABAR,
			DC_SET_GS1_DATABAR,
			DC_GET_GS1_COMPOSITE,
			DC_SET_GS1_COMPOSITE,
			DC_GET_MICRO_PDF417,
			DC_SET_MICRO_PDF417,
			DC_GET_BARCODE_READ_MODE,
			DC_SET_BARCODE_READ_MODE,
			DC_GET_SQRC,
			DC_SET_SQRC,
			DC_GET_SQRC_KEY_UNMATCH,
			DC_SET_SQRC_KEY_UNMATCH,
			DC_GET_SQRC_KEY_MATCH,
			DC_SET_SQRC_KEY_MATCH,
			DC_GET_IQRCODE_SQUARE,
			DC_SET_IQRCODE_SQUARE,
			DC_GET_IQRCODE_RECT,
			DC_SET_IQRCODE_RECT,
			DC_GET_AZTEC_FULL,
			DC_SET_AZTEC_FULL,
			DC_GET_AZTEC_COMPACT,
			DC_SET_AZTEC_COMPACT,
			DC_GET_MENU_READ,
			DC_SET_MENU_READ,
			DC_GET_COMM_SEQ,
			DC_SET_COMM_SEQ,
			DC_GET_BAUD_RATE,
			DC_SET_BAUD_RATE,
			DC_GET_CODE_MARK,
			DC_SET_CODE_MARK,
			DC_GET_LINE_NUM,
			DC_SET_LINE_NUM,
			DC_GET_BCC,
			DC_SET_BCC,
			DC_GET_CTS_SIGNAL,
			DC_SET_CTS_SIGNAL,
			DC_GET_CTS_TIME,
			DC_SET_CTS_TIME,
			DC_GET_ACK_NAK_TIME,
			DC_SET_ACK_NAK_TIME,
			DC_GET_RECV_HDR,
			DC_SET_RECV_HDR,
			DC_GET_RECV_TERMINATOR,
			DC_GET_SEND_TERMINATOR,
			DC_GET_BUZZER,
			DC_SET_BUZZER,
			DC_GET_READ_ERR_BUZZER,
			DC_SET_READ_ERR_BUZZER,
			DC_GET_MAGIC_KEY,
			DC_SET_MAGIC_KEY,
			DC_GET_POWER_ON_BUZZER,
			DC_SET_POWER_ON_BUZZER,
			DC_GET_BUZZER_OFF,
			DC_SET_BUZZER_OFF
		} DeviceCommand;

		typedef void (__stdcall *ScanHandler)(void *userObj, Text::CString code);

	private:
		NotNullPtr<Text::String> devName;

	public:
		CodeScanner(Text::CString devName);
		virtual ~CodeScanner();

		NotNullPtr<Text::String> GetDevName() const;

		virtual void SetCurrMode(ModeType currMode) = 0;
		virtual Bool SoftReset() = 0;
		virtual Bool ResetDefault() = 0;
		virtual void HandleCodeScanned(ScanHandler hdlr, void *userObj) = 0;

		virtual UOSInt GetCommandList(Data::ArrayList<DeviceCommand> *cmdList) = 0;
		virtual Text::CString GetCommandName(DeviceCommand dcmd) = 0;
		virtual CommandType GetCommandParamType(DeviceCommand dcmd, Int32 *minVal, Int32 *maxVal) = 0;
		virtual Text::CString GetCommandParamName(DeviceCommand dcmd, Int32 cmdVal) = 0;
		virtual Int32 GetCommand(DeviceCommand dcmd) = 0;
		virtual Bool SetCommand(DeviceCommand dcmd, Int32 val) = 0;
	};
}
#endif
