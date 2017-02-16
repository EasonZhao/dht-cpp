#include "common.h"
#include <boost/random/random_device.hpp>
#include <boost/random.hpp>

string randString() {
    std::string chars(
        "abcdefghijklmnopqrstuvwxyz"
        "1234567890");
    /*<< We use __random_device as a source of entropy, since we want
    passwords that are not predictable.
    >>*/
    boost::random::random_device rng;
    /*<< Finally we select 8 random characters from the
    string and print them to cout.
    >>*/
    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    string str;
    for (int i = 0; i < 20; ++i) {
        str.append(&(chars[index_dist(rng)]), 1);
    }
    return str;
}