namespace Media
{
	class MPEG4V
	{
	public:
		static Bool GetPAR(UInt8 *frame, Int32 frameSize, Int32 *arh, Int32 *arv);
	};
};
