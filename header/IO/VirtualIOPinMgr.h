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
		struct PinStatus
		{
			UInt32 pinNum;
			Sync::Mutex mut;
			OSInt useCnt;
			Bool pullHigh;
			Int32 outputCnt;
			Bool outputHigh;
		};
	private:
		PinStatus **pins;
		UOSInt pinCnt;

	public:
		VirtualIOPinMgr(UOSInt pinCnt);
		~VirtualIOPinMgr();

		Optional<IO::IOPin> CreatePin(UInt32 pinNum);
		UOSInt GetAvailablePins(Data::ArrayList<Int32> *pinList);
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
		virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);
	};
}
#endif
