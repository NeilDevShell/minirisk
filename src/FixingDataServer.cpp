#include "FixingDataServer.h"
#include "Macros.h"
#include "Streamer.h"
#include <limits>
#include <fstream>
#include <ostream>


namespace minirisk {
	FixingDataServer::FixingDataServer(const std::string& filename) {
		std::ifstream is(filename);
		MYASSERT(!is.fail(), "Could not open file " << filename);
		std::string name;
		string tmp_date;
		double value;
		Date date;
		
		while (is >> name >> tmp_date >> value) {

			if (tmp_date.length() == 8) {
				unsigned y = std::atoi(tmp_date.substr(0, 4).c_str());
				unsigned m = std::atoi(tmp_date.substr(4, 2).c_str());
				unsigned d = std::atoi(tmp_date.substr(6, 2).c_str());
				date.init(y, m, d);
			}
			else
				MYASSERT( true, "Cannot recoginze date input " << tmp_date);

			std::cout << name << "," << date << "," << value << std::endl;
			auto ins = m_FixingData.emplace(std::pair<std::string, Date>(name, date), value);
		} 
	}

	double FixingDataServer::get(const std::string& name, const Date& t) const
	{
		std::pair<std::string, Date> id(name, t);
		auto iter = m_FixingData.find(id);
		MYASSERT(iter != m_FixingData.end(), "fixing data not found");
		return iter->second;
	}
	std::pair<double, bool> FixingDataServer::lookup(const string& name, const Date& t) const
	{
		std::pair<std::string, Date> id(name, t);
		auto iter = m_FixingData.find(id);
		return (iter != m_FixingData.end())
			? std::make_pair(iter->second, true)
			: std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
	}


}