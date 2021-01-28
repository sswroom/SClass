#ifndef _SM_IO_VIRTUALIOPINMGR
#define _SM_IO_VIRTUALIOPINMGR
#include "Data/ArrayList.h"
#include "IO/IOPin.h"
#include "Sync/Mutex.h"

namespace IO
{
	class VirtualIOPinMgr
	{
	public:
		typedef struct
		{
			Int32 pinNum;
			Sync::Mutex *mut;
			OSInt useCnt;
			Bool pullHigh;
			Int32 outputCnt;
			Bool outputHigh;
		} PinStatus;
	private:
		PinStatus **pins;
		OSInt pinCnt;

	public:
		VirtualIOPinMgr(OSInt pinCnt);
		~VirtualIOPinMgr();

		IO::IOPin *CreatePin(Int32 pinNum);
		OSInt GetAvailablePins(Data::ArrayList<Int32> *pinList);
	};

	class VirtualIOPin : public IOPin
	{
	private:
		Bool isOutput;
		VirtualIOPinMgr::PinStatus *pinStatus;
	public:
		VirtualIOPin(VirtualIOPinMgr::PinStatus *pinStatus);
		virtual ~VirtualIOPin();

		virtual Bool IsPinHigh();
		virtual Bool IsPinOutput();
		virtual void SetPinOutput(Bool isOutput);
		virtual void SetPinState(Bool isHigh);
		virtual Bool SetPullType(PullType pt);
		virtual UTF8Char *GetName(UTF8Char *buff);
	};
};
#endif
