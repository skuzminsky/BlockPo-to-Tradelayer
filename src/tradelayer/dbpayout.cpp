#include <dbwrapper.h>
#include <util/system.h>
#include <tradelayer/dbpayout.h>
#include <tradelayer/tupleutils.hpp>

struct PayoutData
{
    static const int CURRENT_VERSION = 1;

    int nVersion;
    std::string sAddr;
    uint32_t nAmount;
    uint64_t nTimeStamp;

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(this->nVersion);
        READWRITE(this->sAddr);
        READWRITE(this->nAmount);
        READWRITE(this->nTimeStamp);
    }
    
    PayoutData() { SetNull(); }
    PayoutData(const std::string& addr, uint32_t amount) : nVersion(PayoutData::CURRENT_VERSION), sAddr(addr), nAmount(amount), nTimeStamp(std::time((nullptr))) {}
    PayoutData(const Payout& payout) : nVersion(PayoutData::CURRENT_VERSION), sAddr(payout.sAddr), nAmount(payout.nAmount), nTimeStamp(payout.nTimeStamp) {}

    void SetNull()
    {
        nVersion = PayoutData::CURRENT_VERSION;
        sAddr.clear();
        nAmount = 0;
        nTimeStamp = 0;
    }
};

class LDBPayout
{
    const CDBWrapper& _dbw;
public:
    enum Prefix : char 
    {
        P = 'p',
        S = 's'
    };

    LDBPayout(const CDBWrapper& dbw) : _dbw(dbw) {}

    static inline std::string GetKeyPrefixS(int contractId=0) { return GetKeyPrefix(Prefix::S, contractId); }
    static inline std::string GetKeyS(int contractId, int blockId) { return GetKey(Prefix::S, contractId, blockId); }
    static inline std::string GetKeyPrefixP(int contractId=0) { return GetKeyPrefix(Prefix::P, contractId); }
    static inline std::string GetKeyP(int contractId, int blockId) { return GetKey(Prefix::P, contractId, blockId); }
    static inline std::string GetKeyPrefix(Prefix prefix, int contractId=0) { return GetKey(prefix, contractId, 0);  }
    static inline std::string GetKey(Prefix prefix, int contractId, int blockId)
    {
        char buf[20];
        return snprintf(buf, sizeof(buf), "%c/%04d/%010d", prefix, contractId, blockId), buf;
    }

    static inline bool IsValidKeyPrefix(const std::string& prefixKey, const std::string& key) 
    {
        Prefix p1 = prefixKey.empty() ? (Prefix)key[0] : (Prefix)prefixKey[0];
        Prefix p2 = (Prefix)key[0];
        return p1==p2; 
    }

    inline std::vector<Payout> GetSocializations(int contractId) const { return GetData(GetKeyPrefixS(contractId)); }
    inline std::vector<Payout> GetSocializations() const { return GetData(GetKeyPrefixS()); }
    inline std::vector<Payout> GetPayouts(int contractId) const { return GetData(GetKeyPrefixP(contractId)); }
    inline std::vector<Payout> GetPayouts() const { return GetData(GetKeyPrefixP()); }

    inline std::vector<Payout> GetData(const std::string& keyPrefix) const
    {
        std::unique_ptr<CDBIterator> it(const_cast<CDBWrapper&>(_dbw).NewIterator());
        std::vector<Payout> data;

        for (it->Seek(keyPrefix);it->Valid();it->Next()) {
            std::string key;
            PayoutData value;
            if (!it->GetKey(key) || !IsValidKeyPrefix(keyPrefix, key) || !it->GetValue(value)) {
                break;
            }
            auto k = tl::LC::Parse<char,uint32_t,uint32_t>(key, "/");
            Payout d;
            d.nContractId = std::get<1>(k);
            d.nBlockId = std::get<2>(k);
            d.sAddr = value.sAddr;
            d.nAmount = value.nAmount;
            d.nTimeStamp = value.nTimeStamp;
            data.push_back(d);
        }

        return data;
    }

    inline bool SaveData(Prefix prefix, const Payout& data) { return SaveData(prefix, {data}); }

    inline bool SaveData(Prefix prefix, const std::vector<Payout>& data)
    {
        CDBBatch batch(_dbw);
        for (auto d : data) {
            batch.Write(GetKey(prefix, d.nContractId, d.nBlockId), PayoutData(d));
        }
        return const_cast<CDBWrapper&>(_dbw).WriteBatch(batch);
    }
};

static std::unique_ptr<LDBPayout> MakeDB(bool test=false)
{
    static fs::path ph = test ? fs::temp_directory_path() / fs::unique_path() : GetDataDir() / "OCL_payouts";
    static CDBWrapper dbw(ph, (1 << 20), test, false, false);
    return MakeUnique<LDBPayout>(dbw);
}

DBPayout::DBPayout() : m_db(MakeDB())
{
}

DBPayout::~DBPayout()
{
}

std::vector<Payout> DBPayout::GetData() const { return m_db->GetData(std::string()); }

std::vector<Payout> DBPayout::GetPayouts(int contractId) const { return m_db->GetPayouts(contractId); }
std::vector<Payout> DBPayout::GetPayouts() const { return m_db->GetPayouts(); }

std::vector<Payout> DBPayout::GetSocializations(int contractId) const { return m_db->GetSocializations(contractId); }
std::vector<Payout> DBPayout::GetSocializations() const { return m_db->GetSocializations(); }
    
bool DBPayout::SavePayouts(const std::vector<Payout>& data) { return m_db->SaveData(LDBPayout::Prefix::P, data); }
bool DBPayout::SaveSocializations(const std::vector<Payout>& data) { return m_db->SaveData(LDBPayout::Prefix::S, data); }