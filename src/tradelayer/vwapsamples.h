#ifndef TL_VWAPSAMPLES_H
#define TL_VWAPSAMPLES_H

#include <string>
#include <vector>
#include <map>
#include <set>

class Channel;

namespace tl
{
    using Channels = std::map<std::string,Channel>;
    using P64 = std::pair<int64_t, int64_t>;
    using V64 = std::vector<P64>;

    // Establish if there was a channel trade at the address specified within last N blocks
    bool FindChannelTrade(const Channels& data, const std::string& address, uint32_t pid, int nBlocks);

    // Get VWAP samples
    std::map<int, P64> GetVWAPSamples(const std::map<int, V64>& data, std::initializer_list<int> nBlocks);

    // Anti wash filter
    int64_t AntiWashFilter(const Channels& data, const std::string& address, uint32_t pid);
}

#endif // TL_VWAPSAMPLES_H