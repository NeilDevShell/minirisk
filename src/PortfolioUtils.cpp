#include "Global.h"
#include "PortfolioUtils.h"
#include "TradePayment.h"
#include "TradeFXForward.h"
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>
#include <utility>

const double NaN = std::numeric_limits<double>::quiet_NaN();


namespace minirisk {

void print_portfolio(const portfolio_t& portfolio)
{
    std::for_each(portfolio.begin(), portfolio.end(), [](auto& pt){ pt->print(std::cout); });
}

std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio, const string& base_ccy)
{
    std::vector<ppricer_t> pricers(portfolio.size());
    std::transform( portfolio.begin(), portfolio.end(), pricers.begin()
                  , [=](auto &pt) -> ppricer_t { return pt->pricer(base_ccy); } );
    return pricers;
}

portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt)
{
    portfolio_values_t prices(pricers.size());
    std::transform(pricers.begin(), pricers.end(), prices.begin()
        , [&mkt](auto& pp) ->  std::pair<double,string>
        {
            try { return std::pair<double,string>(pp->price(mkt, mkt.get_fds()),""); } //TODO: this is actually not need since mkt alreay contains fds in our design
            catch(std::exception& e)
            {
                return std::pair<double,string>(NaN, e.what());
            }
        });
    return prices;
}

std::pair<double, portfolio_values_t> portfolio_total(const portfolio_values_t& values)
{
    portfolio_values_t error_set;
    double sum = 0;
    for (auto value : values)
    {
        if (std::isnan(value.first)) {
            error_set.push_back(value);
        }
        else {
            sum += value.first;
        }
    }
    
    return std::make_pair(sum, error_set);


    //return std::accumulate(values.begin(), values.end(), 0.0); 
}

std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t>& pricers, const Market& mkt)
{
	std::vector<std::pair<string, portfolio_values_t>> fx_delta;

	const double bump_size = 0.1 / 100;

	// filter risk factors related to IR
	auto base = mkt.get_risk_factors(fx_spot_prefix + "[A-Z]{3}");

	// Make a local copy of the Market object, because we will modify it applying bumps
	// Note that the actual market objects are shared, as they are referred to via pointers
	Market tmpmkt(mkt);

	for (const auto& d : base) {
		std::vector<std::pair<double, string>> pv_up, pv_dn;
		std::vector<std::pair<string, double>> bumped(1, d);
		fx_delta.push_back(std::make_pair(d.first, std::vector<std::pair<double, string>>(pricers.size())));

		// bump down and price
		bumped[0].second = d.second * ( 1 - bump_size );
		tmpmkt.set_risk_factors(bumped);
		pv_dn = compute_prices(pricers, tmpmkt);

		// bump up and price
		bumped[0].second = d.second * (1 + bump_size); // bump up
		tmpmkt.set_risk_factors(bumped);
		pv_up = compute_prices(pricers, tmpmkt);

		// restore original market state for next iteration
		// (more efficient than creating a new copy of the market at every iteration)
		bumped[0].second = d.second;
		tmpmkt.set_risk_factors(bumped);

		// compute estimator of the derivative via central finite differences
		double dr = 2.0 * bump_size * d.second;
		std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), fx_delta.back().second.begin()
			, [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string>
			{
				if (std::isnan(hi.first)) {
					return std::make_pair(NaN, hi.second);
				}
				if (std::isnan(lo.first)) {
					return std::make_pair(NaN, lo.second);
				}
				return std::make_pair((hi.first - lo.first) / dr, "");
			});
	}

	return fx_delta;
}


std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt)
{
	std::vector<std::pair<string, portfolio_values_t>> pv01;
	
	const double bump_size = 0.01 / 100;

	// filter risk factors related to IR
	auto base = mkt.get_risk_factors(ir_rate_prefix + ".*\\." + "[A-Z]{3}");
	// loop through the base to generate below 
	/*
		curves_by_ccy = {
			"USD" : {"IR.10Y.USD" : 0.02 },
			"EUR" : {"IR.10Y.EUR" : 0.02 },
		}
	*/
	std::map<string, std::map<string, double>> curves_by_ccy; // TODO: will change this to vector improve the performance? Since set_risk_factor needs to take vector
	std::map<string, double> dummy;
	for (const auto& d : base) {

		string ccy = d.first.substr(d.first.size() - 3, 3);
		curves_by_ccy.emplace(ccy, dummy);
		curves_by_ccy[ccy].emplace(d.first, d.second);
	}


	// Make a local copy of the Market object, because we will modify it applying bumps
	// Note that the actual market objects are shared, as they are referred to via pointers
	Market tmpmkt(mkt);

	// compute prices for perturbated markets and aggregate results
	pv01.reserve(curves_by_ccy.size());

	std::map<string, std::map<string, double>>::iterator iter_1;
	for (iter_1 = curves_by_ccy.begin(); iter_1 != curves_by_ccy.end(); iter_1++ )
	{
		std::map<string, double> curve = iter_1->second;
		std::map<string, double>::iterator iter_2; // For example, iter_2 = ["IR.10Y.USD", 0.02 ]

		std::vector<std::pair<double, string>> pv_up, pv_dn;
		pv01.push_back(std::make_pair(ir_rate_prefix + iter_1->first, std::vector<std::pair<double, string>>(pricers.size())));

		// bump down and price
		std::vector<std::pair<string, double>> bumped_dn;
		for (iter_2 = curve.begin(); iter_2 != curve.end(); iter_2++) {
			bumped_dn.push_back(std::make_pair(iter_2->first, iter_2->second - bump_size)); // bump up
		}
		tmpmkt.set_risk_factors(bumped_dn);
		pv_dn = compute_prices(pricers, tmpmkt);

		// bump up and price
		std::vector<std::pair<string, double>> bumped_up;
		for (iter_2 = curve.begin(); iter_2 != curve.end(); iter_2++) {
			
			bumped_up.push_back(std::make_pair(iter_2->first, iter_2->second + bump_size)); // bump up
		}
		tmpmkt.set_risk_factors(bumped_up);
		pv_up = compute_prices(pricers, tmpmkt);

		// restore original market state for next iteration
		// (more efficient than creating a new copy of the market at every iteration)
		std::vector<std::pair<string, double>> bumped;
		for (iter_2 = curve.begin(); iter_2 != curve.end(); iter_2++) {
			bumped.push_back(std::make_pair(iter_2->first, iter_2->second));
		}
		tmpmkt.set_risk_factors(bumped);

		// compute estimator of the derivative via central finite differences
		double dr = 2.0 * bump_size;
		std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin()
			, [dr](std::pair<double, string> hi, std::pair<double, string> lo) -> std::pair<double, string>
			{
				if (std::isnan(hi.first)) {
					return std::make_pair(NaN, hi.second);
				}
				if (std::isnan(lo.first)) {
					return std::make_pair(NaN, lo.second);
				}
				return std::make_pair((hi.first - lo.first) / dr, "");
			});
	}

	return pv01;
}


std::vector<std::pair<string, portfolio_values_t>> compute_pv01_bucketed(const std::vector<ppricer_t>& pricers, const Market& mkt)
{
    std::vector<std::pair<string, portfolio_values_t>> pv01;  // PV01 per trade

    const double bump_size = 0.01 / 100;

    // filter risk factors related to IR
    auto base = mkt.get_risk_factors(ir_rate_prefix + ".*\\."  + "[A-Z]{3}");

    // Make a local copy of the Market object, because we will modify it applying bumps
    // Note that the actual market objects are shared, as they are referred to via pointers
    Market tmpmkt(mkt);

    // compute prices for perturbated markets and aggregate results
    pv01.reserve(base.size());
    for (const auto& d : base) {
        std::vector<std::pair<double,string>> pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped(1, d);
        pv01.push_back(std::make_pair(d.first, std::vector<std::pair<double,string>>(pricers.size())));

        // bump down and price
        bumped[0].second = d.second - bump_size;
        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt);

        // bump up and price
        bumped[0].second = d.second + bump_size; // bump up
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt);
        
        // restore original market state for next iteration
        // (more efficient than creating a new copy of the market at every iteration)
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin()
            , [dr](std::pair<double,string> hi, std::pair<double,string> lo) -> std::pair<double,string> 
            { 
                if (std::isnan(hi.first)) {
                    return std::make_pair(NaN, hi.second);
                }
                if (std::isnan(lo.first)) {
                    return std::make_pair(NaN, lo.second);
                }
                return std::make_pair((hi.first - lo.first) / dr,""); 
            });
    }

    return pv01;
}


ptrade_t load_trade(my_ifstream& is)
{
    string name;
    ptrade_t p;

    // read trade identifier
    guid_t id;
    is >> id;

    if (id == TradePayment::m_id)
        p.reset(new TradePayment);
	else if (id == TradeFXForward::m_id)
		p.reset(new TradeFXForward);
    else
        THROW("Unknown trade type:" << id);

    p->load(is);

    return p;
}

void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio)
{
    // test saving to file
    my_ofstream of(filename);
    for( const auto& pt : portfolio) {
        pt->save(of);
        of.endl();
    }
    of.close();
}

std::vector<ptrade_t> load_portfolio(const string& filename)
{
    std::vector<ptrade_t> portfolio;

    // test reloading the portfolio
    my_ifstream is(filename);
    while (is.read_line())
        portfolio.push_back(load_trade(is));

    return portfolio;
}

void print_price_vector(const string& name, const portfolio_values_t& values)
{
    auto total_value = portfolio_total(values);

    std::cout
        << "========================\n"
        << name << ":\n"
        << "========================\n"
        << "Total:  " << total_value.first
        <<"\n"<<"Errors: "<<total_value.second.size()
        <<"\n"
        << "\n========================\n";

    for (size_t i = 0, n = values.size(); i < n; ++i)
        if (not std::isnan(values[i].first)) { std::cout << std::setw(5) << i << ": " << values[i].first << "\n"; }
        else { std::cout << std::setw(5) << i << ": " << values[i].second << "\n"; }
        

    std::cout << "========================\n\n";
}

} // namespace minirisk
