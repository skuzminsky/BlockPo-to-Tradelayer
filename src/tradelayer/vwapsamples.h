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

    // Get VWAP samples
    std::map<int, P64> GetVWAPSamples(const std::map<int, V64>& data, const std::vector<int>& nBlocks);

    // Anti wash filter
    std::map<int, P64> GetAntiWashSamples(const std::map<int, V64>& data, const Channels& channels, const std::string& address, uint32_t pid, const std::vector<int>& nBlocks);
}

#endif // TL_VWAPSAMPLES_H