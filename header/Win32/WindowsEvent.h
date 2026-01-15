#ifndef _SM_WIN32_WINDOWSEVENT
#define _SM_WIN32_WINDOWSEVENT

namespace Win32
{
	class WindowsEvent
	{
	public:
		enum class EventType
		{
			Success,
			AuditFailure,
			AuditSuccess,
			Error,
			Information,
			Warning
		};
	private:
		void *hand;
		UnsafeArray<const WChar> progName;

	public:
		WindowsEvent(UnsafeArray<const WChar> progName);
		~WindowsEvent();

		Bool WriteEvent(UInt32 eventCode, EventType evtType, const WChar *evtMessage, const UInt8 *buff, UIntOS buffSize);
	};
}
#endif
