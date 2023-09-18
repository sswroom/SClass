#ifndef _SM_IO_SERIALPORT
#define _SM_IO_SERIALPORT
#include "Data/ArrayList.h"
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
		UOSInt portNum;
		UInt32 baudRate;
		ParityType parity;
		Bool flowCtrl;
		Sync::Event rdEvt;
		Sync::Mutex rdMut;
		Bool reading;

		Bool InitStream();
	public:
		static Bool GetAvailablePorts(NotNullPtr<Data::ArrayList<UOSInt>> ports, Data::ArrayList<SerialPortType> *portTypes);
		static Text::CString GetPortTypeName(SerialPortType portType);
		static UOSInt GetPortWithType(Text::CString portName);
		static UOSInt GetUSBPort();
		static UOSInt GetBTPort();
		static UTF8Char *GetPortName(UTF8Char *buff, UOSInt portNum);
		static Bool ResetPort(UOSInt portNum);

		SerialPort(UOSInt portNum, UInt32 baudRate, ParityType parity, Bool flowCtrl);
		virtual ~SerialPort();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);
		Bool HasData();

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
		UOSInt GetPortNum() const;

		Bool IsError() const;
	};
};
#endif
