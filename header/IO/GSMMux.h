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
			GSMMuxPort *obj;
			Sync::Event *evt;
			Data::ArrayList<UInt8*> *data;
		};

	private:
		IO::Stream *stm;
		GSMPort *ports;
		Sync::Event *readEvt;
		Bool reading;
		UInt8 *readBuff;
		Int32 readBuffSize;
		void *readReq;
		Bool checking;
		Bool closing;

		GSMMux();
		Int32 SendATCommand(UInt8 *buffer, Int32 buffSize, UInt8 *outBuffer, Int32 *outSize); //0 = OK, 1 = ERROR, 2 = Stream Error
		UInt8 CalCheck(UInt8 *buff, Int32 buffSize);
	public:
		GSMMux(IO::Stream *stm, Int32 baudRate);
		~GSMMux();
		GSMMuxPort *OpenVPort(); // NULL if no more ports available
		void CloseVPort(GSMMuxPort *port);
		Bool IsError(); // Check if the mux engime get error
		Int32 SendFrame(Int32 channel, const UInt8 *buffer, Int32 size, GSMFrType frType); //0 = Succeed, 1 = Stream Error

		Bool CheckEvents(Int32 timeout); // Call periodically, return true = error
		void ParseCommData();
		GSMMuxPort *HasAnyData(); //NULL = no data
	};

	class GSMMuxPort : public IO::Stream
	{
	private:
		GSMMux::GSMPort *port;
		GSMMux *mux;
		Bool reading;
	public:
		GSMMuxPort(GSMMux *mux, GSMMux::GSMPort *portInfo);
		virtual ~GSMMuxPort();

		Int32 GetChannel();

		virtual Int32 Read(UInt8 *buff, Int32 size);
		virtual Int32 Write(const UInt8 *buff, Int32 size);

		virtual void *BeginRead(UInt8 *buff, Int32 size, Sync::Event *evt);
		virtual Int32 EndRead(void *reqData, Bool toWait);
		virtual void CancelRead(void *reqData);

		virtual void *BeginWrite(const UInt8 *buff, Int32 size, Sync::Event *evt);
		virtual Int32 EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);
		
		virtual Int32 Flush();
		virtual void Close();
	};
}
