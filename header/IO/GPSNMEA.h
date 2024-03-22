#ifndef _SM_IO_GPSNMEA
#define _SM_IO_GPSNMEA
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/CallbackStorage.h"
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
		typedef void (__stdcall *CommandHandler)(AnyType userObj, const UTF8Char *cmd, UOSInt cmdLen);

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
		NotNullPtr<IO::Stream> stm;
	private:
		Bool relStm;
		Data::ArrayList<Data::CallbackStorage<LocationHandler>> hdlrList;
		Sync::RWMutex hdlrMut;
		CommandHandler cmdHdlr;
		AnyType cmdHdlrObj;

		Bool threadRunning;
		Bool threadToStop;
	private:
		virtual void ParseUnknownCmd(const UTF8Char *cmd, UOSInt cmdLen);
		static ParseStatus ParseNMEALine(UTF8Char *line, UOSInt lineLen, NotNullPtr<Map::GPSTrack::GPSRecord3> record, SateRecord *sateRec);
		static UInt32 __stdcall NMEAThread(AnyType userObj);
	public:
		GPSNMEA(NotNullPtr<IO::Stream> stm, Bool relStm);
		virtual ~GPSNMEA();

		virtual Bool IsDown();
		virtual void RegisterLocationHandler(LocationHandler hdlr, AnyType userObj);
		virtual void UnregisterLocationHandler(LocationHandler hdlr, AnyType userObj);
		virtual void ErrorRecover();
		virtual ServiceType GetServiceType();

		void HandleCommand(CommandHandler cmdHdlr, AnyType userObj);

		static UOSInt GenNMEACommand(const UTF8Char *cmd, UOSInt cmdLen, UInt8 *buff);
		static NotNullPtr<Map::GPSTrack> NMEA2Track(NotNullPtr<IO::Stream> stm, Text::CStringNN sourceName);
	};
}
#endif
