#ifndef _SM_MANAGE_CPUINST
#define _SM_MANAGE_CPUINST
namespace Manage
{
	class CPUInst
	{
	public:
		typedef enum
		{
			IT_X86,
			IT_SSE41,
			IT_SSE42,
			IT_AVX,
			IT_AVX2
		} InstructionType;

		static Bool HasInstruction(InstructionType instType);
	};
}

#endif
