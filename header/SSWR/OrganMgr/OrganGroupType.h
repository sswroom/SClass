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
			Text::String *cName;
			Text::String *eName;

		public:
			OrganGroupType(Int32 seq, Text::CString cName, Text::CString eName);
			~OrganGroupType();

			Int32 GetSeq();
			Text::String *GetCName();
			Text::String *GetEName();
			UTF8Char *ToString(UTF8Char *sbuff);
		};
	}
}
#endif
