#pragma once
#include "ICurve.h"
#include "Date.h"

namespace minirisk {

	struct Market;

	struct CurveFXSpot : ICurveFXSpot
	{
		CurveFXSpot(Market* mkt, const string& name); //FX.Spot.GBP.EUR
		virtual double spot();
		//std::pair<string, string> get_fx_spot_pair(const string& name);
		const string get_base_ccy(const string& name);
		const string get_quote_ccy(const string& name);
		const double get_ccy_rate(const string& name);


	private:
		
		Market* m_mkt;
		string m_fx_spot_name;
		string m_base_ccy;
		string m_quote_ccy;
		double m_base_ccy_rate;
		double m_quote_ccy_rate;

	};
	
}