#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"

namespace minirisk {

/* Logic:

m_ccy --> trade ccy
baseccy --> PV ccy 

*/

PricerPayment::PricerPayment(const TradePayment& trd, const string& configuration)
    : m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy()))
    //, m_fx_ccy(trd.ccy() == "USD" ? "" : fx_spot_name(trd.ccy(),"USD")) //  m_fx_ccy(trd.ccy() == baseccy ? "" : fx_spot_name(trd.ccy(), baseccy))
	, m_ccy_pair(configuration==trd.ccy()?"" : fx_spot_name(trd.ccy(), configuration)) // consistent with the convention of m_fx_ccy
{
}

double PricerPayment::price(Market& mkt, const FixingDataServer & fds) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve);
    double df = disc->df(m_dt); // this throws an exception if m_dt<today

	// Convert trade PV to the consistent portfolio reporting ccy, skip if already the same as trade ccy
	if (!m_ccy_pair.empty())
	{
		ptr_fx_spot_curve_t fx_spot = mkt.get_fx_spot(m_ccy_pair);
		df *= fx_spot->spot();
	}
        

    return m_amt * df;
}

} // namespace minirisk


