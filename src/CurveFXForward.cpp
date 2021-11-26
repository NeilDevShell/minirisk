#pragma once
#include "CurveFXForward.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>

namespace minirisk {

	CurveFXForward::CurveFXForward(Market* mkt, const Date& today, const string& name)
		:
		m_mkt(mkt),
		m_base_ccy(get_base_ccy(name)),
		m_quote_ccy(get_quote_ccy(name)),
		m_curve_spot(mkt, fx_spot_name(m_quote_ccy, m_base_ccy)),//changed
		m_today(today),
		m_base_curve_discount(mkt->get_discount_curve(ir_curve_discount_name(m_base_ccy))),
		m_quote_curve_discount(mkt->get_discount_curve(ir_curve_discount_name(m_quote_ccy)))
		//m_base_curvediscount(mkt,today,ir_curve_discount_prefix+m_base_ccy),
		//m_quote_curvediscount(mkt,today,ir_curve_discount_prefix+m_quote_ccy)
	{
	}

	double CurveFXForward::fwd(const Date& future_date) const
	{
		return m_curve_spot.spot() *  m_quote_curve_discount->df(future_date) / m_base_curve_discount->df(future_date);
		//changed
	}

	// transform FX.Forward.EUR.USD to USD
	const string CurveFXForward::get_base_ccy(const string& name)
	{
		return name.substr(15, 3);
	}

	// transform FX.Forward.EUR.USD to EUR
	const string CurveFXForward::get_quote_ccy(const string& name)
	{
		return name.substr(11, 3);
	}


	string CurveFXForward::name() const
	{
		return fx_forward_prefix + m_quote_ccy + "." + m_base_ccy;
	}

	Date CurveFXForward::today() const
	{
		return m_today;
	}

}