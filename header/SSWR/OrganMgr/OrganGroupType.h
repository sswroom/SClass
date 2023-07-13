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
			NotNullPtr<Text::String> cName;
			NotNullPtr<Text::String> eName;

		public:
			OrganGroupType(Int32 seq, Text::CString cName, Text::CString eName);
			~OrganGroupType();

			Int32 GetSeq();
			NotNullPtr<Text::String> GetCName() const;
			NotNullPtr<Text::String> GetEName() const;
			UTF8Char *ToString(UTF8Char *sbuff);
		};
	}
}
#endif
