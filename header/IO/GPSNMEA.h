#ifndef _SM_IO_GPSNMEA
#define _SM_IO_GPSNMEA
#include "AnyType.h"
#include "Data/ArrayListObj.hpp"
#include "Data/CallbackStorage.h"
#include "IO/Stream.h"
#include "Map/LocationService.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"
#include "Text/CString.h"

namespace IO
{
	class GPSNMEA : public Map::LocationService
	{
	public:
		typedef void (CALLBACKFUNC CommandHandler)(AnyType userObj, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);

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
		NN<IO::Stream> stm;
	private:
		Bool relStm;
		Data::ArrayListObj<Data::CallbackStorage<LocationHandler>> hdlrList;
		Sync::RWMutex hdlrMut;
		CommandHandler cmdHdlr;
		AnyType cmdHdlrObj;

		Bool threadRunning;
		Bool threadToStop;
	private:
		virtual void ParseUnknownCmd(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen);
		static ParseStatus ParseNMEALine(UnsafeArray<UTF8Char> line, UOSInt lineLen, NN<Map::GPSTrack::GPSRecord3> record, NN<SateRecord> sateRec);
		static UInt32 __stdcall NMEAThread(AnyType userObj);
	public:
		GPSNMEA(NN<IO::Stream> stm, Bool relStm);
		virtual ~GPSNMEA();

		virtual Bool IsDown();
		virtual void RegisterLocationHandler(LocationHandler hdlr, AnyType userObj);
		virtual void UnregisterLocationHandler(LocationHandler hdlr, AnyType userObj);
		virtual void ErrorRecover();
		virtual ServiceType GetServiceType();

		void HandleCommand(CommandHandler cmdHdlr, AnyType userObj);

		static UOSInt GenNMEACommand(UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, UnsafeArray<UInt8> buff);
		static NN<Map::GPSTrack> NMEA2Track(NN<IO::Stream> stm, Text::CStringNN sourceName);
	};
}
#endif
