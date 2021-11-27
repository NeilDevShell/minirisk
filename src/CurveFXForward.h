#pragma once
#include "ICurve.h"
#include "Date.h"
#include "CurveFXSpot.h"
#include "CurveDiscount.h"

namespace minirisk {

	struct Market;

	struct CurveFXForward : ICurveFXForward
	{
		CurveFXForward(Market* mkt, const Date& today, const string& name);
		virtual double fwd(const Date& future_date) const;
		const string get_base_ccy(const string& name);
		const string get_quote_ccy(const string& name);
		virtual string name() const;
		virtual Date today() const;

	private:
		Market* m_mkt;
		string m_base_ccy;
		string m_quote_ccy;
		ptr_fx_spot_curve_t m_curve_spot;
		Date m_today;
		ptr_disc_curve_t m_base_curve_discount;
		ptr_disc_curve_t m_quote_curve_discount;
	};

}
