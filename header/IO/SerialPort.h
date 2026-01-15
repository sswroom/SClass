#ifndef _SM_IO_SERIALPORT
#define _SM_IO_SERIALPORT
#include "Data/ArrayListNative.hpp"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace IO
{
	class SerialPort : public IO::Stream
	{
	public:
		typedef enum
		{
			SPT_UNKNOWN,
			SPT_SERIALPORT,
			SPT_BLUETOOTH,
			SPT_COM0COM,
			SPT_DWSERIAL,
			SPT_USBSERIAL
		} SerialPortType;

		typedef enum
		{
			PARITY_NONE,
			PARITY_ODD,
			PARITY_EVEN
		} ParityType;
	private:
		void *handle;
		UIntOS portNum;
		UInt32 baudRate;
		ParityType parity;
		Bool flowCtrl;
		Sync::Event rdEvt;
		Sync::Mutex rdMut;
		Bool reading;

		Bool InitStream();
	public:
		static Bool GetAvailablePorts(NN<Data::ArrayListNative<UIntOS>> ports, Data::ArrayListNative<SerialPortType> *portTypes);
		static Text::CStringNN GetPortTypeName(SerialPortType portType);
		static UIntOS GetPortWithType(Text::CStringNN portName);
		static UIntOS GetUSBPort();
		static UIntOS GetBTPort();
		static UnsafeArrayOpt<UTF8Char> GetPortName(UnsafeArray<UTF8Char> buff, UIntOS portNum);
		static Bool ResetPort(UIntOS portNum);

		SerialPort(UIntOS portNum, UInt32 baudRate, ParityType parity, Bool flowCtrl);
		virtual ~SerialPort();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);
		Bool HasData();

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
		UIntOS GetPortNum() const;

		Bool IsError() const;
	};
}
#endif
