namespace Map
{
	class MapUtil
	{
	public:
		static Double CalDistance(Double lat1, Double lon1, Double lat2, Double lon2);
		static Int32 SplitPoints(Double *buff, Double lat1, Double lon1, Double lat2, Double lon2, Double dist);
		static Double PointDir(Double lat1, Double lon1, Double lat2, Double lon2);
	};
}
