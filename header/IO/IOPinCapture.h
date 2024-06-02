#ifndef _SM_IO_IOPINCAPTURE
#define _SM_IO_IOPINCAPTURE
#include "Data/ArrayList.h"
#include "IO/IOPin.h"
#include "Manage/HiResClock.h"

namespace IO
{
	class IOPinCapture : public IO::IOPin
	{
	private:
		struct CaptureBuff
		{
			CaptureBuff *prevBuff;
			UOSInt thisBuffSize;
			UOSInt thisDataCnt;
			Double *times;
			Int32 *isHigh;
		};
	private:
		NN<IO::IOPin> pin;
		Manage::HiResClock clk;
		CaptureBuff *capBuff;
		Double lastTime;
		Bool lastVal;
		Double startTime;

		void FreeCaptureData(CaptureBuff *buff);
		UOSInt AddCaptureData(Data::ArrayList<Double> *times, Data::ArrayList<Int32> *isHigh, CaptureBuff *buff);
	public:
		IOPinCapture(NN<IO::IOPin> pin);
		virtual ~IOPinCapture();

		virtual Bool IsPinHigh();
		virtual Bool IsPinOutput();
		virtual void SetPinOutput(Bool isOutput);
		virtual void SetPinState(Bool isHigh);
		virtual Bool SetPullType(PullType pt);
		virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);

		UOSInt GetCaptureData(Data::ArrayList<Double> *times, Data::ArrayList<Int32> *isHigh);
	};
}
#endif
