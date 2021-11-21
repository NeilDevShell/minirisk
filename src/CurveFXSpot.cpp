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
	m_fx_spot_name(name),
	m_base_ccy(get_base_ccy(name)),
	m_quote_ccy(get_quote_ccy(name)),
	m_base_ccy_rate(get_ccy_rate(mkt, m_base_ccy)),
	m_quote_ccy_rate(get_ccy_rate(mkt, m_quote_ccy)),
	m_today(mkt->today())
{

}

double CurveFXSpot::spot() const
{
	/*
	tricky:
	1. check any of the cross is USD.
	2. the order
*/
		
	return  m_base_ccy_rate / m_quote_ccy_rate;

}

// transform FX.Spot.EUR.USD to USD
const string CurveFXSpot::get_base_ccy(const string& name)
{
	std::cout << name.substr(9, 3) << std::endl;
	return name.substr(8,3);
}

// transform FX.Spot.EUR.USD to USD
const string CurveFXSpot::get_quote_ccy(const string& name)
{
	std::cout << name.substr(13, 3) << std::endl;
	return name.substr(12, 3);
}

//
const double CurveFXSpot::get_ccy_rate(Market* mkt, const string& name)
{
	if ( name == "USD" )
		return 1.0;

	string postfix(".USD");
	return mkt->get_fx_spot(fx_spot_prefix + name + postfix);
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
