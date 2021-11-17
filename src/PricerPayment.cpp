#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"

namespace minirisk {

/* Logic:

m_ccy --> trade ccy
baseccy --> PV ccy 

*/

PricerPayment::PricerPayment(const TradePayment& trd)
    : m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy()))
    , m_fx_ccy(trd.ccy() == "USD" ? "" : fx_spot_name(trd.ccy(),"USD")) //  m_fx_ccy(trd.ccy() == baseccy ? "" : fx_spot_name(trd.ccy(), baseccy))
{
}

double PricerPayment::price(Market& mkt) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve);
    double df = disc->df(m_dt); // this throws an exception if m_dt<today

    // This PV is expressed in m_ccy. It must be converted in USD.
	if (!m_fx_ccy.empty())
	{
		double fx_rate = mkt.get_fx_spot(m_fx_ccy); //FX.SPOT.EUR.GBP --> FX.SPOT.EUR / FX.SPOT.GBP
		df *= fx_rate;
	}
        

    return m_amt * df;
}

} // namespace minirisk


