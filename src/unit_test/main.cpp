#include <gtest/gtest.h>
#include "dht/krpc.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

//{"t":"aa", "y":"r", "r": {"id":"abcdefghij0123456789", "token":"aoeusnth", "values": ["axje.u", "idhtnm"]}}
TEST(PTREE, JSON)
{
    ptree root;
    root.put("t", "aa");
    root.put("y", "r");
    //root.put("q", "ping");
    ptree r;
    r.put("id", "abcdefghij0123456789");
    r.put("token", "aoeusnth");
    ptree values;
    ptree val1, val2;
    val1.put("", "axje.u");
    val2.put("", "idhtnm");
    values.push_back(make_pair("", val1));
    values.push_back(make_pair("", val2));
    r.push_back(make_pair("values", values));

    root.push_back(make_pair("r", r));

    //pt.put();
    std::ostringstream buf;
    write_json(buf, root, false);
    std::string json = buf.str();
    EXPECT_TRUE(json == "{\"t\":\"aa\",\"y\":\"r\",\"r\":{\"id\":\"abcdefghij0123456789\",\"token\":\"aoeusnth\",\"values\":[\"axje.u\",\"idhtnm\"]}}\n");
}

//{"t":"aa", "y":"q", "q":"ping", "a":{"id":"abcdefghij0123456789"}}
TEST(PINGQUERY, ENCODE)
{
    ptree root;
    root.put("t", "aa");
    root.put("y", "q");
    root.put("q", "ping");

    ptree a;
    a.put("id", "abcdefghij0123456789");
    root.push_back(make_pair("a", a));

    string result = dht::encode(root);
    EXPECT_TRUE(result == "d1:ad2:id20:abcdefghij0123456789e1:q4:ping1:t2:aa1:y1:qe");
}

TEST(PINGQUERY, DECODE)
{
    string code("d1:ad2:id20:abcdefghij0123456789e1:q4:ping1:t2:aa1:y1:qe");
    ptree root = dht::decode(code);
    EXPECT_TRUE(root.get<string>("t") == "aa");
    EXPECT_TRUE(root.get<string>("y") == "q");
    EXPECT_TRUE(root.get<string>("q") == "ping");
    ptree a = root.get_child("a");
    EXPECT_TRUE(a.get<string>("id") == "abcdefghij0123456789");
}

TEST(PINGRESPONSE, DECODE)
{
    string code("d1:rd2:id20:mnopqrstuvwxyz123456e1:t2:aa1:y1:re");
    ptree root = dht::decode(code);
    EXPECT_TRUE(root.get<string>("t") == "aa");
    EXPECT_TRUE(root.get<string>("y") == "r");
    ptree r = root.get_child("r");
    EXPECT_TRUE(r.get<string>("id") == "mnopqrstuvwxyz123456");
}

TEST(FIND_NODEQUERY, ENCODE)
{
    ptree root;
    root.put("t", "aa");
    root.put("y", "q");
    root.put("q", "find_node");
    ptree a;
    a.put("id", "abcdefghij0123456789");
    a.put("target", "mnopqrstuvwxyz123456");
    root.push_back(make_pair("a", a));

    string result = dht::encode(root);
    EXPECT_TRUE(result == "d1:ad2:id20:abcdefghij01234567896:target20:mnopqrstuvwxyz123456e1:q9:find_node1:t2:aa1:y1:qe");
}

TEST(FIND_NODERESPONSE, DECODE)
{
    string code("d1:rd2:id20:0123456789abcdefghij5:nodes9:def456...e1:t2:aa1:y1:re");
    ptree root = dht::decode(code);
    EXPECT_TRUE(root.get<string>("t") == "aa");
    EXPECT_TRUE(root.get<string>("y") == "r");
    ptree r = root.get_child("r");
    //EXPECT_TRUE(pkt.dic_name == "r");
    EXPECT_TRUE(r.get<string>("id") == "0123456789abcdefghij");
    EXPECT_TRUE(r.get<string>("nodes") == "def456...");
}

//{"t":"aa", "y":"q", "q":"get_peers", "a": {"id":"abcdefghij0123456789", "info_hash":"mnopqrstuvwxyz123456"}}
TEST(GET_PEERSRQUERY, ENCODE)
{
    ptree root;
    root.put("t", "aa");
    root.put("y", "q");
    root.put("q", "get_peers");
    ptree a;
    a.put("id", "abcdefghij0123456789");
    a.put("info_hash", "mnopqrstuvwxyz123456");
    root.push_back(make_pair("a", a));

    string result = dht::encode(root);
    EXPECT_TRUE(result == "d1:ad2:id20:abcdefghij01234567899:info_hash20:mnopqrstuvwxyz123456e1:q9:get_peers1:t2:aa1:y1:qe");
}

TEST(GET_PEERSRESPONSE_CLOSEST_NODES, DECODE)
{
    string code("d1:rd2:id20:abcdefghij01234567895:nodes9:def456...5:token8:aoeusnthe1:t2:aa1:y1:re");
    ptree root = dht::decode(code);
    EXPECT_TRUE(root.get<string>("t") == "aa");
    EXPECT_TRUE(root.get<string>("y") == "r");
    //EXPECT_TRUE(pkt.contact["q"] == "get_peers");
    ptree r = root.get_child("r");
    EXPECT_TRUE(r.get<string>("id") == "abcdefghij0123456789");
    EXPECT_TRUE(r.get<string>("token") == "aoeusnth");
    EXPECT_TRUE(r.get<string>("nodes") == "def456...");
}

template <typename T>
std::vector<T> as_vector(ptree const& pt, ptree::key_type const& key)
{
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}

//{"t":"aa", "y":"r", "r": {"id":"abcdefghij0123456789", "token":"aoeusnth", "values": ["axje.u", "idhtnm"]}}
TEST(GET_PEERSRESPONSE_WITH_PEERS, DECODE)
{
    string code("d1:rd2:id20:abcdefghij01234567895:token8:aoeusnth6:valuesl6:axje.u6:idhtnmee1:t2:aa1:y1:re");
    ptree root = dht::decode(code);
    ptree r = root.get_child("r");
    EXPECT_TRUE(r.get<string>("id") == "abcdefghij0123456789");
    EXPECT_TRUE(r.get<string>("token") == "aoeusnth");
    EXPECT_TRUE(root.get<string>("t") == "aa");
    EXPECT_TRUE(root.get<string>("y") == "r");
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, r.get_child("values.")) {
        EXPECT_TRUE(v.second.data() == "axje.u" || v.second.data() == "idhtnm");
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}