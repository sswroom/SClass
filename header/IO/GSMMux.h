#ifndef _SM_IO_GSMMUX
#define _SM_IO_GSMMUX
#include "Data/ArrayListArr.h"

namespace IO
{
	class GSMMuxPort;

	//Dynamic Memory allocation
	//Always check data periodically
	//Make use of events

	class GSMMux
	{
	public:
		enum GSMFrType
		{
			SABM = 47,
			UA = 99,
			DM = 15,
			DISC = 67,
			UIH = 239,
			UI = 3,
			PF = 16
		};

		struct GSMPort
		{
			Int32 portId;
			Bool opened;
			Optional<GSMMuxPort> obj;
			Optional<Sync::Event> evt;
			NN<Data::ArrayListArr<UInt8>> data;
		};

	private:
		Optional<IO::Stream> stm;
		UnsafeArray<GSMPort> ports;
		NN<Sync::Event> readEvt;
		UnsafeArray<UInt8> readBuff;
		Int32 readBuffSize;
		Optional<IO::StreamReadReq> readReq;
		Bool checking;
		Bool closing;

		GSMMux();
		Int32 SendATCommand(UnsafeArray<UInt8> buffer, Int32 buffSize, UnsafeArrayOpt<UInt8> outBuffer, OptOut<UIntOS> outSize); //0 = OK, 1 = ERROR, 2 = Stream Error
		UInt8 CalCheck(UnsafeArray<const UInt8> buff, UIntOS buffSize);
	public:
		GSMMux(NN<IO::Stream> stm, Int32 baudRate);
		~GSMMux();
		Optional<GSMMuxPort> OpenVPort();
		void CloseVPort(NN<GSMMuxPort> port);
		Bool IsError(); // Check if the mux engime get error
		Int32 SendFrame(Int32 channel, UnsafeArray<const UInt8> buffer, UIntOS size, GSMFrType frType); //0 = Succeed, 1 = Stream Error

		Bool CheckEvents(Int32 timeout); // Call periodically, return true = error
		void ParseCommData();
		Optional<GSMMuxPort> HasAnyData();
	};

	class GSMMuxPort : public IO::Stream
	{
	private:
		NN<GSMMux::GSMPort> port;
		NN<GSMMux> mux;
		Bool reading;
	public:
		GSMMuxPort(NN<GSMMux> mux, NN<GSMMux::GSMPort> portInfo);
		virtual ~GSMMuxPort();

		Int32 GetChannel();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);
		
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
