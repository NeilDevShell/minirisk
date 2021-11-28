#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

	/*
	  FX.SPOT.EUR.USD
	  EUR: base ccy
	  USD: quote ccy
	*/


CurveFXSpot::CurveFXSpot(Market* mkt, const Date& today, const string& name)
	: 
	m_fx_spot_name(name),
	m_base_ccy(get_base_ccy(name)),
	m_quote_ccy(get_quote_ccy(name)),
	m_base_ccy_rate(get_ccy_rate(mkt, m_base_ccy)),
	m_quote_ccy_rate(get_ccy_rate(mkt, m_quote_ccy)),
	m_today(today)
{

}

double CurveFXSpot::spot() const
{	
	return  m_base_ccy_rate / m_quote_ccy_rate;
}

// transform FX.SPOT.EUR.USD to EUR
const string CurveFXSpot::get_base_ccy(const string& name)
{
	return name.substr(8,3);
}

// transform FX.SPOT.EUR.USD to USD
const string CurveFXSpot::get_quote_ccy(const string& name)
{
	return name.substr(12, 3);
}

const double CurveFXSpot::get_ccy_rate(Market* mkt, const string& name)
{
	if ( name == "USD" )
		return 1.0;

	return mkt->get_fx_spot_rate(fx_spot_name(name, "USD"));
}

string CurveFXSpot::name() const
{
	return m_fx_spot_name;
}

Date CurveFXSpot::today() const
{
	return m_today;
}

}
