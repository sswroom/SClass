#ifndef _SM_SSWR_ORGANMGR_ORGANGROUPTYPE
#define _SM_SSWR_ORGANMGR_ORGANGROUPTYPE

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganGroupType
		{
		private:
			Int32 seq;
			const UTF8Char *cName;
			const UTF8Char *eName;

		public:
			OrganGroupType(Int32 seq, const UTF8Char *cName, const UTF8Char *eName);
			~OrganGroupType();

			Int32 GetSeq();
			const UTF8Char *GetCName();
			const UTF8Char *GetEName();
			UTF8Char *ToString(UTF8Char *sbuff);
		};
	}
}
#endif
