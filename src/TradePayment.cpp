#include "TradePayment.h"
#include "PricerPayment.h"

namespace minirisk {

ppricer_t TradePayment::pricer(const string& configuration) const
{
    return ppricer_t(new PricerPayment(*this, configuration));
}

} // namespace minirisk
