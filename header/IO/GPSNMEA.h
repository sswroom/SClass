#ifndef _SM_IO_GPSNMEA
#define _SM_IO_GPSNMEA
#include "Data/ArrayList.h"
#include "Map/ILocationService.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"
#include "Text/CString.h"

namespace IO
{
	class GPSNMEA : public Map::ILocationService
	{
	public:
		typedef void (__stdcall *CommandHandler)(void *userObj, const UTF8Char *cmd, UOSInt cmdLen);

	private:
		enum class ParseStatus
		{
			NotNMEA,
			Unsupported,
			Handled,
			NewRecord
		};

		struct SateRecord
		{
			UOSInt sateCnt;
			SateStatus sates[32];
		};
	protected:
		IO::Stream *stm;
	private:
		Bool relStm;
		Data::ArrayList<LocationHandler> *hdlrList;
		Data::ArrayList<void *> *hdlrObjs;
		Sync::RWMutex *hdlrMut;
		CommandHandler cmdHdlr;
		void *cmdHdlrObj;

		Bool threadRunning;
		Bool threadToStop;
	private:
		virtual void ParseUnknownCmd(const UTF8Char *cmd);
		static ParseStatus ParseNMEALine(UTF8Char *line, UOSInt lineLen, Map::GPSTrack::GPSRecord2 *record, SateRecord *sateRec);
		static UInt32 __stdcall NMEAThread(void *userObj);
	public:
		GPSNMEA(IO::Stream *stm, Bool relStm);
		virtual ~GPSNMEA();

		virtual Bool IsDown();
		virtual void RegisterLocationHandler(LocationHandler hdlr, void *userObj);
		virtual void UnregisterLocationHandler(LocationHandler hdlr, void *userObj);
		virtual void ErrorRecover();
		virtual ServiceType GetServiceType();

		void HandleCommand(CommandHandler cmdHdlr, void *userObj);

		static UOSInt GenNMEACommand(const UTF8Char *cmd, UOSInt cmdLen, UInt8 *buff);
		static Map::GPSTrack *NMEA2Track(IO::Stream *stm, Text::CString sourceName);
	};
}
#endif
