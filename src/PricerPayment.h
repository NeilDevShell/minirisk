#pragma once

#include "IPricer.h"
#include "TradePayment.h"

namespace minirisk {

struct PricerPayment : IPricer
{
    PricerPayment(const TradePayment& trd, const string& configuration);

    virtual double price(Market& m) const;

private:
    double m_amt;
    Date   m_dt;
    string m_ir_curve;
    string m_fx_ccy;
	string m_ccy_pair;
};

//TODO: simplify it by creating base class - why the m_base_ccy cannot pass to PricerPayment
//struct Pricer : IPricer
//{
//	//PricerPayment(const TradePayment& trd);
//	//virtual double price(Market& m) const;
//
//private:
//	string m_base_ccy; // convert all reported PV to this ccy
//};

} // namespace minirisk

