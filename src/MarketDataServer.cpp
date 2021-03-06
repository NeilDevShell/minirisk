#include "MarketDataServer.h"
#include "Macros.h"
#include "Streamer.h"

#include <limits>

namespace minirisk {

// transforms FX.SPOT.EUR.USD into FX.SPOT.EUR
string mds_spot_name(const string& name)
{
    // NOTE: in a real system error checks should be stricter, not just on the last 3 characters
    MYASSERT((name.substr(name.length() - 3, 3) == "USD"),
        "Only FX pairs in the format FX.SPOT.CCY.USD can be queried. Got " << name);
    return name.substr(0, name.length() - 4);
}

// construct a dummy marketdataserver based on m_risk_factors
MarketDataServer::MarketDataServer(std::map<string, double>risk_fsctors)
{
	m_data = risk_fsctors;
}

MarketDataServer::MarketDataServer(const string& filename)
{
    std::ifstream is(filename);
    MYASSERT(!is.fail(), "Could not open file " << filename);
    do {
        string name;
        double value;
        is >> name >> value;
        //std::cout << name << " " << value << "\n";
        auto ins = m_data.emplace(name, value);
        MYASSERT(ins.second, "Duplicated risk factor: " << name);
    } while (is); // TODO: original code is wrong and always reads one extra line
}

double MarketDataServer::get(const string& name) const
{
	auto iter = m_data.find(name);
	MYASSERT(iter != m_data.end(), "Market data not found: " << name);
	return iter->second;
}


std::map<std::string, double> MarketDataServer::getCurve(const string& name) const
{
	//auto iter = m_data.find(name);
	// generate regex based on curve name
	const string regex = "IR\\..*\\.";
	const string ccy = name.substr(ir_rate_prefix.length(), 3);

	// Here, instead finding the forward rate which has exactly the same indentifier, eg., IR.USD,
	// we return an array in which the identifier of an element matches IR.*.CCY
	//std::vector<std::string> tenors = match(regex+ccy);
	std::map<std::string, double> curve = match_curve(regex + ccy);

	//MYASSERT(iter != m_data.end(), "Market data not found: " << name);
	return curve;
}

std::pair<double, bool> MarketDataServer::lookup(const string& name) const
{
    auto iter = m_data.find(name);
    return (iter != m_data.end())  // found?
            ? std::make_pair(iter->second, true)
            : std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
}

std::vector<std::string> MarketDataServer::match(const std::string& expr) const
{
	std::vector<std::string> result;
	std::regex r(expr);
	std::map<string, double>::iterator it;
	for (auto const& iter : m_data)
	{
		if (std::regex_match(iter.first, r))
		{
			result.push_back(iter.first.substr(3, iter.first.length() - 7)); //Get the tenor only
		}
	}


	MYASSERT(result.size() != 0, "Could not find any matched tenor in the MarketDataSever for " << expr);

	return result;
}

std::map<std::string, double> MarketDataServer::match_curve(const std::string& expr) const
{
	std::map<std::string, double> result;
	std::regex r(expr);
	//std::map<string, double>::iterator it;
	for (auto const& iter : m_data)
	{
		if (std::regex_match(iter.first, r))
		{
			//const std::string tenor = iter.first.substr(3, iter.first.length() - 7);
			const double rate = iter.second;
			result[iter.first] = rate;
		}
	}

	MYASSERT(result.size() != 0, "Could not find any matched tenor in the MarketDataSever for " << expr);

	return result;
}

} // namespace minirisk

