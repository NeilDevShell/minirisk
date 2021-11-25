#include "TradeFXForward.h"
#include "PricerFXForward.h"

namespace minirisk {

ppricer_t TradeFXForward::pricer(const string& configuration) const
{
    return ppricer_t(new PricerFXForward(*this, configuration));
}

} // namespace minirisk
