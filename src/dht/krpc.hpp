#ifndef DHT_KRPC_HPP
#define DHT_KRPC_HPP

#include <string>
#include <map>
#include <boost/format.hpp>
#include <boost/integer.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>

using namespace std;

namespace dht {
    string encode(boost::property_tree::ptree& root) {
        string code;
        boost::property_tree::ptree a = root.get_child("a");
        if (!a.empty()) {
            code += "d";
            code += "1:a";
            code += "d";
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v, a) {
                
                code += to_string(v.first.size());
                code += ":";
                code += v.first.data();

                code += to_string(v.second.data().size());
                code += ":";
                code += v.second.data();
            }
        }

        code += "e";
        code += "1:q";
        code += to_string(root.get<string>("q").size());
        code += ":";
        code += root.get<string>("q");

        code += "1:t";
        code += to_string(root.get<string>("t").size());
        code += ":";
        code += root.get<string>("t");

        code += "1:y";
        code += to_string(root.get<string>("y").size());
        code += ":";
        code += root.get<string>("y");
        code += "e";

        return code;
    }

    string get_key(string::const_iterator &iter) {
        string::const_iterator iter2 = iter;
        while (*iter2 != ':') iter2++;
        size_t len = boost::lexical_cast<size_t>(string(iter, iter2));
        iter = iter2 + 1;
        string key = string(iter, iter + len);
        iter += len;
        return key;
    }
    string get_value(string::const_iterator &iter) {
        string::const_iterator iter2 = iter;
        while (*iter2 != ':') iter2++;
        size_t len = boost::lexical_cast<size_t>(string(iter, iter2));
        iter = iter2 + 1;
        string value = string(iter, iter + len);
        iter += len;
        return value;
    }

    boost::property_tree::ptree decode(string code) {
        boost::property_tree::ptree root;
        string::const_iterator iter = code.begin();
        if (*iter == 'd') iter++;
        string key = get_key(iter);
        boost::property_tree::ptree a;
        boost::property_tree::ptree r;
        boost::property_tree::ptree *node = &root;
        
        while (*iter != 'e') {
            if (*iter == 'd') {
                iter++;
                if (key == "r") {
                    node = &r;
                } else if (key == "a") {
                    node = &a;
                }
            } else {
                string key, value;
                key = get_key(iter);
                if (*iter == 'l') {
                    iter++;
                    boost::property_tree::ptree values;
                    while (*iter != 'e') {
                        value = get_value(iter);
                        boost::property_tree::ptree val;
                        val.put("", value);
                        values.push_back(make_pair("", val));
                    }
                    iter++;
                    node->push_back(make_pair(key, values));
                    break;
                } else {
                    value = get_value(iter);
                }
                node->put(key, value);
            }
        }
        if (node == &a) 
            root.push_back(make_pair("a", a));
        if (node == &r) 
            root.push_back(make_pair("r", r));
        //
        iter++;
        while (*iter != 'e' && iter != code.end()) {
            key = get_key(iter);
            string value = get_value(iter);
            root.put(key, value);
        }
        
        return root;
    }
} // End namespace dht

#endif DHT_PGRPC_HPP
