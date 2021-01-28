#ifndef _SM_IO_IOPIN
#define _SM_IO_IOPIN

namespace IO
{
	class IOPin
	{
	public:
		typedef enum
		{
			PT_DISABLE,
			PT_UP,
			PT_DOWN
		} PullType;
	public:
		virtual ~IOPin() {};

		virtual Bool IsPinHigh() = 0;
		virtual Bool IsPinOutput() = 0;
		virtual void SetPinOutput(Bool isOutput) = 0;
		virtual void SetPinState(Bool isHigh) = 0;
		virtual Bool SetPullType(PullType pt) = 0;
		virtual UTF8Char *GetName(UTF8Char *buff) = 0;
	};
}
#endif
