#include "PricerFXForward.h"
#include "TradeFXForward.h"
#include "CurveDiscount.h"
#include "CurveFXForward.h"
#include "CurveFXSpot.h"

namespace minirisk {

/* Logic:

m_ccy --> trade ccy
baseccy --> PV ccy 

*/

PricerFXForward::PricerFXForward(const TradeFXForward& trd, const string& configuration)
: m_amt(trd.quantity())
, m_strike(trd.strike())
, m_fixing_date(trd.fixing_date())
, m_delivery_date(trd.delivery_date())
, m_ir_curve(ir_curve_discount_name(trd.quote_ccy()))
, m_fx_forward_curve(fx_forward_name(trd.base_ccy(), trd.quote_ccy()))
, m_ccy_1(trd.base_ccy())
, m_ccy_2(trd.quote_ccy())
, m_ccy_pair(m_ccy_2 == configuration ? "" : fx_spot_name(m_ccy_2, configuration))
{
}

double PricerFXForward::price(Market& mkt, const FixingDataServer & fds) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve);
    double df = disc->df(m_delivery_date); // this throws an exception if m_dt<today

	// Date logic here
	Date t0 = mkt.today();
	double fx_forward_rate = 0;
	if (t0 < m_fixing_date)
	{
		ptr_fx_forward_curve_t fx_forward_crv = mkt.get_fx_forward(m_fx_forward_curve);
		fx_forward_rate = fx_forward_crv->fwd(m_fixing_date);
	}
	else if (t0 == m_fixing_date)
	{
		std::pair<double, bool> fixing = fds.lookup(fx_spot_name(m_ccy_1, m_ccy_2), m_fixing_date);
		if (fixing.second)
			fx_forward_rate = fixing.first;
		else
		{
			ptr_fx_forward_curve_t fx_forward_crv = mkt.get_fx_forward(m_fx_forward_curve);
			fx_forward_rate = fx_forward_crv->fwd(m_fixing_date);
		}
	}
	else if ((t0 > m_fixing_date&& t0 < m_delivery_date) || (t0 == m_delivery_date))
		fx_forward_rate = fds.get(fx_spot_name(m_ccy_1, m_ccy_2), m_fixing_date);
	else
		THROW("Trade dates are illegal: pricing date is " << t0 << ", fixing date is " << m_fixing_date << ", and settlement date is " << m_delivery_date);


	df *= (fx_forward_rate - m_strike);

	// Convert trade PV to the consistent portfolio reporting ccy, skip if already the same as trade ccy
	if (!m_ccy_pair.empty())
	{
		ptr_fx_spot_curve_t fx_spot = mkt.get_fx_spot(m_ccy_pair);
		df *= fx_spot->spot();
	}

    return m_amt * df;
}

} // namespace minirisk


