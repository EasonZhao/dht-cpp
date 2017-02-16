#ifndef DHT_KRPC_HPP
#define DHT_KRPC_HPP

#include <string>
#include <map>
#include <boost/format.hpp>
#include <boost/integer.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

namespace dht {
    static boost::uint16_t counter = 1;

    class KRPCFrame {
        typedef map<string, string> ParamDic;
    protected:
        string tid_;
        ParamDic params_;

    public:
        virtual string encode() { return ""; }
    };
    

    class KRPCPingQuery : public KRPCFrame {
    public:
        KRPCPingQuery(string node_id) {
            params_["nodeid"] = node_id;
            std::stringstream ss;
            ss << std::hex << counter++;
            ss >> tid_;
            if (tid_.size() == 1)
                tid_.insert(0, 1, '0');
        }

        string encode() {
            boost::format fmt("d1:ad2:id20:%1%e1:q4:ping1:t2:%2%1:y1:qe");
            fmt % params_["nodeid"] % tid_;
            return fmt.str();
        }
    };

    class KRPCPingResponse : public KRPCFrame {
    public:
        KRPCPingResponse(string id, string tid) {
            params_["id"] = id;
            tid_ = tid;
        }

        string tid() {
            return tid_;
        }

        string value(string key) {
            return params_[key];
        }

        //d1:rd2:id20:mnopqrstuvwxyz123456e1:t2:aa1:y1:re
        static KRPCPingResponse decode(string buf) {
            boost::iterator_range<string::iterator> rge;
            rge = boost::find_first(buf, "id20:");
            string target(rge.end(), rge.end()+20);
            rge = boost::find_first(buf, "t2:");
            string tid = string(rge.end(), rge.end() + 2);
            return KRPCPingResponse(target, tid);
        }
    private:
        string target_;
    };

    class KRPCFindNodeQuery : public KRPCFrame {
    public:
        KRPCFindNodeQuery(string nodeid, string target) {
            params_["nodeid"] = nodeid;
            params_["target"] = target;
        }

        string encode() {
            boost::format fmt("d1:ad2:id20:%1%6:target20:%2%e1:q9:find_node1:t2:%3%1:y1:qe");
            fmt % params_["id"] % params_["target"] % tid_;
            return fmt.str();
        }
    };

} // End namespace dht

#endif DHT_PGRPC_HPP
