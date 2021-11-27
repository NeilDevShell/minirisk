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
	std::map<int, double> calculateCurve(const std::map<string, double>& original_curve);
	std::map<int, double> interpolateCurve(const std::map<int, double>& curve_calculated);


private:
    Date   m_today;
    string m_name;
    //double m_rate;
	std::map<string, double> m_curve;
	std::map<int, double> m_curve_calculated;
	std::map<int, double> m_curve_interpolated;

};

} // namespace minirisk
