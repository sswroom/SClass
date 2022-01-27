#ifndef _SM_IO_TVCONTROL
#define _SM_IO_TVCONTROL
#include "Data/ArrayList.h"
#include "Text/CString.h"

namespace IO
{
	class TVControl
	{
	public:
		typedef enum
		{
			CT_POWERON,
			CT_POWEROFF,
			CT_FORCEPOWEROFF,
			CT_GETPOWERSTATUS,
			CT_AUTOSETUP,
			CT_FACTORYRESET,
			CT_SCREENRESET,
			CT_VOLUMEUP,
			CT_VOLUMEDOWN,
			CT_AUDIOMUTE,
			CT_INPUT_DSUB,
			CT_INPUT_VIDEO,
			CT_INPUT_HDMI1,
			CT_INPUT_HDMI2,
			CT_INPUT_DVI_D,
			CT_INPUT_DISPLAYPORT,
			CT_INPUT_DISPLAYPORT2,
			CT_INPUT_DISPLAYPORT3,
			CT_INPUT_YPBPR,
			CT_INPUT_YPBPR2,
			CT_INPUT_SVIDEO,
			CT_GETCURRINPUT,
			CT_GETDATETIME,
			CT_SETCURRDATETIME,
			CT_SELFDIAGNOSIS,
			CT_GETSERIALNO,
			CT_GETMODEL,
			CT_GETFIRMWARE,
			CT_GETFIRMWARE2,
			CT_GETPOWERSAVEMODE,
			CT_GETPOWERSAVETIME,
			CT_GETAUTOSTANDBYTIME,
			CT_GETLANMAC,
			CT_GETLANIP,
			CT_GETTEMPERATURE1,
			CT_GETTEMPERATURE2,
			CT_GETTEMPERATURE3,
			CT_GETBACKLIGHT,
			CT_SETBACKLIGHT,
			CT_GETCONTRAST,
			CT_SETCONTRAST,
			CT_GETSHARPNESS,
			CT_SETSHARPNESS,
			CT_GETBRIGHTNESS,
			CT_SETBRIGNTNESS,
			CT_GETHUE,
			CT_SETHUE,
			CT_GETSATURATION,
			CT_SETSATURATION,
			CT_GETCOLORTEMP,
			CT_SETCOLORTEMP,
			CT_IRLOCK,
			CT_IRUNLOCK,
			CT_GETIRSTATUS,
			CT_GETHPOS,
			CT_GETVPOS
		} CommandType;

		typedef enum
		{
			CF_INSTRUCTION,
			CF_GETCOMMAND,
			CF_SETCOMMAND
		} CommandFormat;

		typedef enum
		{
			TVT_MDT701S,
			TVT_NEC
		} TVType;

		typedef struct
		{
			TVType tvType;
			const UTF8Char *name;
			UInt32 defBaudRate;
		} TVInfo;
	protected:
		TVControl();
	public:
		virtual ~TVControl();

		virtual Bool SendInstruction(CommandType ct) = 0;
		virtual Bool SendGetCommand(CommandType ct, Int32 *val, UTF8Char *sbuff) = 0;
		virtual Bool SendSetCommand(CommandType ct, Int32 val) = 0;
		virtual void GetSupportedCmd(Data::ArrayList<CommandType> *cmdList, Data::ArrayList<CommandFormat> *cmdFormats) = 0;

		static OSInt GetTVList(Data::ArrayList<TVType> *tvList);
		static Bool GetTVInfo(TVType tvType, TVInfo *info);
		static TVControl *CreateTVControl(IO::Stream *stm, TVType tvType, Int32 monId);
		static Text::CString GetCommandName(CommandType cmdType);
	};
}
#endif
