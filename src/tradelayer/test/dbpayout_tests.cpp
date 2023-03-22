#include <test/test_bitcoin.h>
#include <boost/test/unit_test.hpp>

#include <tradelayer/dbpayout.h>

BOOST_FIXTURE_TEST_SUITE(tradelayer_dbpayout_tests, BasicTestingSetup)

static inline Payout GetPayout(int cid, int i)
{
    Payout p;
    p.nContractId = cid;
    p.nBlockId = i;
    p.sAddr = InsecureRand256().ToString();
    p.nAmount = i*i*i;
    p.nTimeStamp = std::time(nullptr);
    return p;
}
BOOST_AUTO_TEST_CASE(simple)
{
    const int cid = 1;
    std::vector<Payout> pa;
    for (int i=0;i<10;++i) pa.push_back(GetPayout(cid,i));

    DBPayout db;

    BOOST_CHECK(db.SavePayouts(pa));

    auto pb = db.GetPayouts();
    BOOST_CHECK(std::equal(pa.begin(), pa.end(), pb.begin()));

    pb = db.GetPayouts(cid);
    BOOST_CHECK(std::equal(pa.begin(), pa.end(), pb.begin()));

    pb = db.GetPayouts(cid+1);
    BOOST_CHECK_EQUAL(pb.size(), 0);

    auto sc = db.GetSocializations();
    BOOST_CHECK_EQUAL(sc.size(), 0);

    sc = db.GetSocializations(cid);
    BOOST_CHECK_EQUAL(sc.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()