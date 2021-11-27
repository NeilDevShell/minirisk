#pragma once

#include "Trade.h"

namespace minirisk {

/*
	T1 - Fixing date
	T2 - Delivery date
	ccy1 - quote ccy
	ccy2 - base ccy
*/

struct TradeFXForward : Trade<TradeFXForward>
{
    friend struct Trade<TradeFXForward>;

    static const guid_t m_id;
    static const std::string m_name;

	TradeFXForward() {}

    void init(double quantity, const std::string& ccy_1, const std::string& ccy_2, const double strike, const Date& fixing_date, const Date& delivery_date)
    {
        Trade::init(quantity);
		m_ccy_1 = ccy_1;
		m_ccy_2 = ccy_2;
		m_strike = strike;
		m_fixing_date = fixing_date;
		m_delivery_date = delivery_date;
    }

    virtual ppricer_t pricer(const string& configuration) const;

	const double& strike() const
	{
		return m_strike;
	}

    const string& quote_ccy() const
    {
        return m_ccy_2;
    }

	const string& base_ccy() const
	{
		return m_ccy_1;
	}

    const Date& delivery_date() const
    {
        return m_delivery_date;
    }

	const Date& fixing_date() const  
	{
		return m_fixing_date;
	}

private:
    void save_details(my_ofstream& os) const
    {
        os << m_ccy_1 << m_ccy_2 << m_strike << m_fixing_date  << m_delivery_date;
    }

    void load_details(my_ifstream& is)
    {
		is >> m_ccy_1 >> m_ccy_2 >> m_strike >> m_fixing_date >> m_delivery_date;

    }

    void print_details(std::ostream& os) const
    {
		os << format_label("Strike level") << m_strike << std::endl;
		os << format_label("Base Currency") << m_ccy_1 << std::endl;
        os << format_label("Quote Currency") << m_ccy_2 << std::endl;
		os << format_label("Fixing Date") << m_fixing_date << std::endl;
		os << format_label("Settlement Date") << m_delivery_date << std::endl;
    }

private:
	string m_ccy_1;
	string m_ccy_2;
	double m_strike;
	Date m_fixing_date;
    Date m_delivery_date;
	
};

} // namespace minirisk
