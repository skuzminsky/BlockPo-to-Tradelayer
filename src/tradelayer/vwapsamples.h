#ifndef TL_VWAPSAMPLES_H
#define TL_VWAPSAMPLES_H

#include <string>
#include <vector>
#include <map>
#include <set>

class Channel;

namespace tl
{
    using P64 = std::pair<int64_t, int64_t>;
    using V64 = std::vector<P64>;

    std::vector<int64_t> get_vwap1(const std::map<std::string,Channel>& data, const std::string& address, uint32_t pid, int nBlocks);

    std::map<int, P64> get_vwap2(const std::map<int, V64>& data, std::initializer_list<int> nBlocks);

    int64_t get_vwap3();
}

#endif // TL_VWAPSAMPLES_H