#pragma once

#include<map>
#include<regex>
//#include<utility>
#include "Global.h"
#include "Date.h"

namespace minirisk {

	struct FixingDataServer
	{
	public:
		FixingDataServer(const std::string& filename);

		double get(const std::string& name, const Date& t) const;
		std::pair<double, bool> lookup(const std::string& name, const Date& t) const;

	private:
		std::map<std::pair<std::string, Date>, double> m_FixingData;
	};

}