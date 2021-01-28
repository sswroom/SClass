#ifndef _SM_WIN32_WINDOWSEVENT
#define _SM_WIN32_WINDOWSEVENT

namespace Win32
{
	class WindowsEvent
	{
	public:
		typedef enum
		{
			ET_SUCCESS,
			ET_AUDIT_FAILURE,
			ET_AUDIT_SUCCESS,
			ET_ERROR,
			ET_INFORMATION,
			ET_WARNING
		} EventType;
	private:
		void *hand;
		const WChar *progName;

	public:
		WindowsEvent(const WChar *progName);
		~WindowsEvent();

		Bool WriteEvent(UInt32 eventCode, EventType evtType, const WChar *evtMessage, const UInt8 *buff, OSInt buffSize);
	};
};
#endif
