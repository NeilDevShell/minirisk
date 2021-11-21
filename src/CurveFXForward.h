#pragma once
#include "ICurve.h"
#include "Date.h"
#include "CurveFXSpot.h"
#include "CurveDiscount.h"

namespace minirisk {

	struct Market;

	struct CurveFXForward : ICurveFXForward
	{
		CurveFXForward(Market* mkt, const string& name, const Date& today, const Date& future_date);
		virtual double forward();
		const string get_base_ccy(const string& name);
		const string get_quote_ccy(const string& name);
		const double get_spot(const CurveFXSpot& curve_spot);

	private:
		Market* m_mkt;
		CurveFXSpot m_curve_spot;
		string m_base_ccy;
		string m_quote_ccy;
		Date m_today;
		Date m_future_date;
		ptr_disc_curve_t m_base_curve_discount;
		ptr_disc_curve_t m_quote_curve_discount;
	};

}
