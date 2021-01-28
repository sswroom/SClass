namespace IO
{
	struct DATASEG
	{
		Int64 offset;
		Int32 length;
	};

	class DataSegment
	{
	private:
		IO::StmData::FileData *fd;
		Int32 capacity;
		Int32 dataCount;

	public:
		DataSegment(IO::StmData::FileData *fd);
		~DataSegment();
		IO::StmData::FileData *GetFd();
		void Add(Int64 offset, Int32 length);
		Int32 GetCount();
		Int64 GetOffset(Int32 i);
		Int32 GetLength(Int32 i);

		DATASEG *datas;
	};
};
