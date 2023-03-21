#include <tradelayer/vwapsamples.h>
#include <tradelayer/tradelayer.h>
#include <tradelayer/rpctxobject.h>

namespace tl
{

namespace mc = mastercore;

static inline float GetBlockVolatility(const std::map<int, P64>& data, int block)
{
    auto p = data.find(block);
    if (p != data.end())
    {
        auto pv = p->second;
        return pv.first / pv.second;       
    }
    return 0;
}

static inline bool FindChannelTrade(const Channels& data, const std::string& address, uint32_t pid, int nBlocks)
{
    // Build set of blocks for this trade 
    std::set<int> blocks;
    std::vector<uint256> txArray;
    {
        LOCK(cs_tally);
        std::string ch;
        if (!mc::t_tradelistdb->checkChannelRelation(address, ch))
        {
            // hasn't traded in a channel
            return false;
        }
        mc::t_tradelistdb->getTradesForAddress(address, txArray, pid);
    }
    for(auto tx : txArray) 
    {
        UniValue v(UniValue::VOBJ);
        if (0 == populateRPCTransactionObject(tx, v))
        {
            blocks.emplace(v["block"].get_int());
        }
    }

    // Assume no trade has been found
    if (blocks.empty())
    {
        return false;
    }

    // Sort channels by block
    std::vector<Channel> channels;
    std::for_each(data.begin(), data.end(), [&](const std::pair<std::string,Channel>& p) { channels.push_back(p.second); });
    std::sort(channels.begin(), channels.end(), [](const Channel& a, const Channel& b) { return a.getLastBlock() < b.getLastBlock(); });

    int n = 0, b = 0;
    for (auto c = channels.rbegin(); c != channels.rend(); ++c) 
    {
        // Count blocks
        if (c->getLastBlock() != b) 
        {
            b = c->getLastBlock();
            ++n;
        }

        if (!c->isPartOfChannel(address)) 
        {
            continue;
        }
                
        if (blocks.find(b) != blocks.end())
        {
            return true;
        }

        if (n > nBlocks-1) break;
    }

    return false;
}

static inline std::map<int, P64> GetVWAPSamples(const std::map<int, V64>& data, const std::vector<int>& nBlocks, std::function<bool(int n)> pred)
{
    std::map<int, P64> vwap;

    for (size_t n : nBlocks) 
    {
        if (data.size() < n) {
            break;
        }

        if (pred(n))
        {
            continue;
        }

        int64_t _min = std::numeric_limits<int64_t>::max();
        int64_t _max = std::numeric_limits<int64_t>::min();

        for (auto end=data.cend(), start=std::next(end,-n); start!=end; std::next(start)) 
        {
            auto& v64 = const_cast<V64&>(start->second);
            if (v64.size()) 
            {
                std::sort(v64.begin(), v64.end(), [](const P64& a, const P64& b) { return a.first < b.first; });
                
                if (int64_t m = std::prev(v64.cend())->first > _max) _max = m;
                if (int64_t m = v64.cbegin()->first < _min) _min = m;
            }
        }

        vwap[n] = std::make_pair(_min, _max);
    }
    
    return vwap;
}

std::map<int, P64> GetVWAPSamples(const std::map<int, V64>& data, const std::vector<int>& nBlocks)
{
    return GetVWAPSamples(data, nBlocks, [](int){ return false; });
}

std::map<int, P64> GetAntiWashSamples(const std::map<int, V64>& data, const Channels& channels, const std::string& address, uint32_t pid, const std::vector<int>& nBlocks)
{
    return GetVWAPSamples(data, nBlocks, [&](int n){ return FindChannelTrade(channels, address, pid, n); });
}

} // namespace tl