#include <iostream>
#include <vector>

#include "spider.h"
#include "krpc.hpp"
#include "common.h"

using namespace dht;

dht::Spider::Spider(boost::asio::io_service & io_s) 
    : sock_(io_s, udp::endpoint(udp::v4(), 0))
    , nodeid_(randString())
    , io_s_(io_s)
{
}

dht::Spider::~Spider()
{
}

bool dht::Spider::joinDHT()
{
    udp::resolver resolver(io_s_);
    udp::endpoint endpoint = *(resolver.resolve({ udp::v4(), "router.bittorrent.com", "6881" }));

    // send ping
    KRPCPingQuery q(nodeid_);
    string data = q.encode();
    sock_.send_to(boost::asio::buffer(data.c_str(), data.size()), endpoint);

    // wait for respons
    udp::endpoint sender_endpoint;
    vector<char> buf(1024);
    size_t reply_length = sock_.receive_from(boost::asio::buffer(&buf[0], 1024), sender_endpoint);
    KRPCPingResponse response = KRPCPingResponse::decode(string(buf.begin(), buf.end()));
    target_node_id_ = response.value("id");

    std::cout << "target_node_id_ = " << target_node_id_ << std::endl;
    return true;
}

