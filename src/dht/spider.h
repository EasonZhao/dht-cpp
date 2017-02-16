#ifndef DHT_SPIDER_H
#define DHT_SPIDER_H

#include "krpc.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>

using boost::asio::ip::udp;

namespace dht {
    class Spider
    {
    public:
        Spider(boost::asio::io_service &io_s);
        ~Spider();

        bool joinDHT();

    private:
        udp::socket sock_;
        string nodeid_;
        string target_node_id_;
        boost::asio::io_service &io_s_;
    };

}// End namespace dht

#endif // !DHT_SPIDER_H


