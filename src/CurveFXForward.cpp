#pragma once
#include "CurveFXForward.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>

namespace minirisk {

	CurveFXForward::CurveFXForward(Market* mkt, const string& name, const Date& today, const Date& future_date)
		:
		m_mkt(mkt),
		m_curve_spot(mkt, name),//changed
		m_base_ccy(get_base_ccy(name)),
		m_quote_ccy(get_quote_ccy(name)),
		m_today(today),
		m_future_date(future_date),
		m_base_curve_discount(mkt->get_discount_curve(m_base_ccy)),
		m_quote_curve_discount(mkt->get_discount_curve(m_quote_ccy))
		//m_base_curvediscount(mkt,today,ir_curve_discount_prefix+m_base_ccy),
		//m_quote_curvediscount(mkt,today,ir_curve_discount_prefix+m_quote_ccy)
	{
	}

	double CurveFXForward::forward()
	{
		return m_curve_spot.spot() * m_base_curve_discount->df(m_future_date) / m_quote_curve_discount->df(m_future_date);
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

	const double CurveFXForward::get_spot(const CurveFXSpot& curve_spot)
	{
		return curve_spot.spot();
	}
}