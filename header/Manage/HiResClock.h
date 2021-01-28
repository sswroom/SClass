#ifndef _SM_MANAGE_HIRESCLOCK
#define _SM_MANAGE_HIRESCLOCK
namespace Manage
{
	class HiResClock
	{
	private:
		Int64 stTime;
	public:
		HiResClock();
		~HiResClock();

		void Start();
		Double GetTimeDiff();
		Int64 GetTimeDiffus();
		Double GetAndRestart();

		static void Init();
		static Int64 GetRelTime_us();
	};
};
#endif
