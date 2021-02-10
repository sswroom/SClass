#ifndef _SM_DATA_USABLE
#define _SM_DATA_USABLE

namespace Data
{
	class Usable
	{
	public:
		virtual void BeginUse() = 0;
		virtual void EndUse() = 0;
	};
}
#endif
