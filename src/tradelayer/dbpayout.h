#ifndef PAYOUT_DBWRAPPER_H
#define PAYOUT_DBWRAPPER_H

#include <memory>
#include <string>
#include <vector>

struct Payout
{
    int nContractId;
    int nBlockId;
    std::string sAddr;
    uint32_t nAmount;
    uint32_t nTimeStamp;

    friend inline bool operator==(const Payout& a, const Payout& b) { return a.nContractId==b.nContractId && a.nBlockId==b.nBlockId && a.sAddr==b.sAddr && a.nAmount==b.nAmount && a.nTimeStamp==b.nTimeStamp; }
    friend inline bool operator!=(const Payout& a, const Payout& b) { return !(a==b); }

    inline std::string ToString() const 
    {
        char buf[64];
        return snprintf(buf, sizeof(buf), "%d/%d/%s/%u/%u", nContractId, nBlockId, sAddr.substr(0,8).c_str(), nAmount, nTimeStamp), buf; 
    }
};

class LDBPayout;

class DBPayout
{
    std::unique_ptr<LDBPayout> m_db;

public:
    DBPayout();
    ~DBPayout();

    std::vector<Payout> GetData() const;

    std::vector<Payout> GetPayouts(int contractId) const;
    std::vector<Payout> GetPayouts() const;
    std::vector<Payout> GetSocializations(int contractId) const;
    std::vector<Payout> GetSocializations() const;

    bool SavePayouts(const std::vector<Payout>& data);
    bool SaveSocializations(const std::vector<Payout>& data);
};

#endif // PAYOUT_DBWRAPPER_H