#ifndef _SM_MANAGE_PROCESSENUMERATOR
#define _SM_MANAGE_PROCESSENUMERATOR
namespace Manage
{
	class ProcessEnumerator
	{
	private:
		void *hand;
		void *procEntry;
		Bool first;

	private:
		ProcessEnumerator(void *hand);
	public:
		~ProcessEnumerator();

		Bool NextItem();
		UInt32 GetProcId();
		UInt32 GetParentProcId();
		UInt32 GetThreadCnt();
		WChar *GetFileName(WChar *buff);

		static ProcessEnumerator *EnumAllProcesses();
	};
};
#endif
