namespace Media
{
	class MPEG4V
	{
	public:
		static Bool GetPAR(UnsafeArray<UInt8> frame, Int32 frameSize, OutParam<Int32> arh, OutParam<Int32> arv);
	};
};
