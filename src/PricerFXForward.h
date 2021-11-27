#pragma once

#include "IPricer.h"
#include "TradeFXForward.h"

namespace minirisk {

struct PricerFXForward : IPricer
{
	PricerFXForward(const TradeFXForward& trd, const string& configuration);

    virtual double price(Market& m, const FixingDataServer& fds) const;

private:
    double m_amt;
	double m_strike;
	Date   m_fixing_date;
	Date   m_delivery_date;
    string m_ir_curve; //ccy2
	string m_fx_forward_curve; //ccy1
	string m_ccy_1;
	string m_ccy_2;
	string m_ccy_pair;
};

} // namespace minirisk

