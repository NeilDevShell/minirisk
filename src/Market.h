#pragma once

#include "Global.h"
#include "IObject.h"
#include "ICurve.h"
#include "MarketDataServer.h"
#include "FixingDataServer.h"
#include <vector>
#include <regex>

namespace minirisk {

struct Market : IObject
{
private:
    // NOTE: this function is not thread safe
    template <typename I, typename T>
    std::shared_ptr<const I> get_curve(const string& name);

    double from_mds(const string& objtype, const string& name);
	std::map<string, double> curve_from_mds(const string& objtype, const string& name);
	std::map<string, double> curve_from_risk_factors(const string& name);

public:

    typedef std::pair<string, double> risk_factor_t;
    typedef std::vector<std::pair<string, double>> vec_risk_factor_t;

    Market(const std::shared_ptr<const MarketDataServer>& mds, const Date& today)
        : m_today(today)
        , m_mds(mds)
    {
    }
	Market(const std::shared_ptr<const MarketDataServer>& mds, const Date& today, const std::shared_ptr<const FixingDataServer>& fds )
		: m_today(today)
		, m_mds(mds)
		, m_fds(fds)
	{
	}

    virtual Date today() const { return m_today; }

    // get an object of type ICurveDisocunt
    const ptr_disc_curve_t get_discount_curve(const string& name);

	// get an object of type ICurveFXForward
	const ptr_fx_forward_curve_t get_fx_forward(const string& name);

    // yield rate for currency name
    const double get_yield(const string& name);

	// get yield tenors and values for currency name
	const std::map<string, double> get_yield_curve(const string& name);

    // fx exchange rate to convert 1 unit of ccy1 into USD
    const double get_fx_spot(const string& ccy);

    // after the market has been disconnected, it is no more possible to fetch
    // new data points from the market data server
    void disconnect()
    {
        m_mds.reset();
    }

    // returns risk factors matching a regular expression
    vec_risk_factor_t get_risk_factors(const std::string& expr) const;

    // clear all market curves execpt for the data points
    void clear()
    {
        std::for_each(m_curves.begin(), m_curves.end(), [](auto& p) { p.second.reset(); });
    }

    // destroy all existing objects and modify a selected number of data points
    void set_risk_factors(const vec_risk_factor_t& risk_factors);

	// return fixing data server
	const FixingDataServer get_fds() { return *m_fds; };

private:
    Date m_today;
    std::shared_ptr<const MarketDataServer> m_mds;
	std::shared_ptr<const FixingDataServer> m_fds;

    // market curves
    std::map<string, ptr_curve_t> m_curves;

    // raw risk factors
    std::map<string, double> m_risk_factors;
};

} // namespace minirisk

