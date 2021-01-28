namespace Map
{
	class Geotag
	{
	private:
		static Bool SetDouble(Int32 *buff, Double val);
	public:
		static void SetPosition(Media::JPEGExif *exif, Double lat, Double lon);
	};
}
