#pragma once
#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

	/*
	  Example:EURUSD - The price of EUR in USD
	  USD - Base CCY
	  EUR - Quote CCY
	*/


CurveFXSpot::CurveFXSpot(Market* mkt, const string& name)
	: 
	m_mkt(mkt),
	m_fx_spot_name(name),
	m_base_ccy(get_base_ccy(name)),
	m_quote_ccy(get_base_ccy(name)),
	m_base_ccy_rate(get_ccy_rate(name)),
	m_quote_ccy_rate(get_ccy_rate(name))
{

}

double CurveFXSpot::spot()
{
	/*
	tricky:
	1. check any of the cross is USD.
	2. the order
*/
		
	return m_quote_ccy_rate / m_base_ccy_rate;

}

// transform FX.Spot.EUR.USD to USD
const string CurveFXSpot::get_base_ccy(const string& name)
{
	return name.substr(9,3);
}

// transform FX.Spot.EUR.USD to USD
const string CurveFXSpot::get_quote_ccy(const string& name)
{
	return name.substr(13, 3);
}

//
const double CurveFXSpot::get_ccy_rate(const string& name)
{
	if ( name == "USD" )
		return 1.0;

	string prefix("FX.Spot.");
	string postfix("USD");
	return m_mkt->get_fx_spot(prefix + name + postfix);
}
}
