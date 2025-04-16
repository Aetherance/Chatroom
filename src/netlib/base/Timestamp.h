#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include<iostream>
#include <vector>
#include<time.h>

namespace ilib{
namespace base{

class Timestamp
{
public:
    explicit Timestamp(int64_t microSecondsSinceEpoch_);
    static Timestamp now();
    time_t secondsSinceEpoch() const;
    int64_t microSecondsSinceEpoch() const;
    bool valid() const;
    bool operator==(const Timestamp) const;
    bool operator<(const Timestamp) const;
    bool operator>(const Timestamp) const;
    bool operator<=(const Timestamp) const;
    bool operator>=(const Timestamp) const;

    static double timeDifference(Timestamp high, Timestamp low);
    static Timestamp addTime(Timestamp timestamp,double seconds);

    ~Timestamp() = default;
private:
    int64_t microSecondsSinceEpoch_;
};

}
}

#endif