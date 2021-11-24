#include "Market.h"
#include "CurveDiscount.h"

#include <vector>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)
{
    ptr_curve_t& curve_ptr = m_curves[name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(name);
}

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}

std::map<string, double> Market::curve_from_mds(const string& objtype, const string& name)
{
	// If m_risk_factor is not empty and m_mds is empty, then we create curve from the market data in m_risk_factors
	if (!m_risk_factors.empty() && !m_mds )
		return curve_from_risk_factors(name);

	std::map<string, double> curve = m_mds->getCurve(name); //["10Y", 0.03] TODO: think about how to optimize
	for (auto tenor_value : curve) // Example: this_tenor = ["IR.10Y.USD", 0.03]
		m_risk_factors.emplace(tenor_value.first, tenor_value.second);
	return curve;
}

std::map<string, double> Market::curve_from_risk_factors(const string& name)
{
	std::shared_ptr<const MarketDataServer> tmp_mds(new MarketDataServer(m_risk_factors));
	std::map<string, double> curve = tmp_mds->getCurve(name); //["10Y", 0.03] TODO: think about how to optimize
	//for (auto tenor_value : curve) // Example: this_tenor = ["IR.10Y.USD", 0.03]
	//	m_risk_factors.emplace(tenor_value.first, tenor_value.second);
	return curve;
}

const std::map<string, double> Market::get_yield_curve(const string& ccyname)
{
	string name(ir_rate_prefix + ccyname);
	return curve_from_mds("yield curve", name);
};

const double Market::get_yield(const string& ccyname)
{
    string name(ir_rate_prefix + ccyname);
    return from_mds("yield curve", name);
};

const double Market::get_fx_spot(const string& name)
{
    return from_mds("fx spot", mds_spot_name(name));
}

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& expr) const
{
    vec_risk_factor_t result;
    std::regex r(expr);
    for (const auto& d : m_risk_factors)
        if (std::regex_match(d.first, r))
            result.push_back(d);
    return result;
}

} // namespace minirisk
