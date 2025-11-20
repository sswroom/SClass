#ifndef _SM_IO_IOPINCAPTURE
#define _SM_IO_IOPINCAPTURE
#include "Data/ArrayList.hpp"
#include "IO/IOPin.h"
#include "Manage/HiResClock.h"

namespace IO
{
	class IOPinCapture : public IO::IOPin
	{
	private:
		struct CaptureBuff
		{
			Optional<CaptureBuff> prevBuff;
			UOSInt thisBuffSize;
			UOSInt thisDataCnt;
			UnsafeArray<Double> times;
			UnsafeArray<Int32> isHigh;
		};
	private:
		NN<IO::IOPin> pin;
		Manage::HiResClock clk;
		NN<CaptureBuff> capBuff;
		Double lastTime;
		Bool lastVal;
		Double startTime;

		void FreeCaptureData(NN<CaptureBuff> buff);
		UOSInt AddCaptureData(NN<Data::ArrayList<Double>> times, NN<Data::ArrayList<Int32>> isHigh, NN<CaptureBuff> buff);
	public:
		IOPinCapture(NN<IO::IOPin> pin);
		virtual ~IOPinCapture();

		virtual Bool IsPinHigh();
		virtual Bool IsPinOutput();
		virtual void SetPinOutput(Bool isOutput);
		virtual void SetPinState(Bool isHigh);
		virtual Bool SetPullType(PullType pt);
		virtual UnsafeArray<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);

		UOSInt GetCaptureData(NN<Data::ArrayList<Double>> times, NN<Data::ArrayList<Int32>> isHigh);
	};
}
#endif
