#ifndef _SM_SSWR_ORGANMGR_ORGANGROUPTYPE
#define _SM_SSWR_ORGANMGR_ORGANGROUPTYPE
#include "Text/CString.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganGroupType
		{
		private:
			Int32 seq;
			NN<Text::String> cName;
			NN<Text::String> eName;

		public:
			OrganGroupType(Int32 seq, Text::CStringNN cName, Text::CStringNN eName);
			~OrganGroupType();

			Int32 GetSeq();
			NN<Text::String> GetCName() const;
			NN<Text::String> GetEName() const;
			UnsafeArray<UTF8Char> ToString(UnsafeArray<UTF8Char> sbuff);
		};
	}
}
#endif
