#ifndef _SM_DATA_CONDITION
#define _SM_DATA_CONDITION

namespace Data
{
	enum class CompareCondition
	{
		Unknown,
		Equal,
		GreaterOrEqual,
		LessOrEqual,
		Greater,
		Less,
		NotEqual
	};
}
#endif
