#pragma once
#include "ICurve.h"
#include <map>

namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    // compute the discount factor
    double df(const Date& t) const;

    virtual Date today() const { return m_today; }
	std::map<Date, double> calculateCurve(std::map<string, double> original_curve, const Date& today);

private:
    Date   m_today;
    string m_name;
    //double m_rate;
	std::map<string, double> m_curve;
	std::map<Date, double> m_curve_calculated;
};

} // namespace minirisk
