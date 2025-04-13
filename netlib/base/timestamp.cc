#include"Timestamp.h"

using namespace ilib::base;

Timestamp::Timestamp(int64_t microSecondsSinceEpoch_) {
    this->microSecondsSinceEpoch_ = microSecondsSinceEpoch_;
}

Timestamp Timestamp::now() {
    timespec spec;
    clock_gettime(CLOCK_REALTIME,&spec);
    int64_t microtime = (spec.tv_sec * 1000000) + (spec.tv_nsec / 1000);
    return Timestamp(microtime);
}

time_t Timestamp::secondsSinceEpoch() const{
    return microSecondsSinceEpoch_ / 1000000;
}

int64_t Timestamp::microSecondsSinceEpoch() const {
    return microSecondsSinceEpoch_;
}

bool Timestamp::valid() const {
    return microSecondsSinceEpoch_ > 0;
}

bool Timestamp::operator==(const Timestamp that_time) const {
    return microSecondsSinceEpoch_ == that_time.microSecondsSinceEpoch_;
}

bool Timestamp::operator<(const Timestamp that_time) const{
    return microSecondsSinceEpoch_ < that_time.microSecondsSinceEpoch_;
}

bool Timestamp::operator>(const Timestamp that_time) const{
    return microSecondsSinceEpoch_ > that_time.microSecondsSinceEpoch_;
}

bool Timestamp::operator<=(const Timestamp that_time) const{
    return microSecondsSinceEpoch_ <= that_time.microSecondsSinceEpoch_;
}

bool Timestamp::operator>=(const Timestamp that_time) const{
    return microSecondsSinceEpoch_ >= that_time.microSecondsSinceEpoch_;
}

double Timestamp::timeDifference(Timestamp high,Timestamp low) {
    return (high.microSecondsSinceEpoch_ - low.microSecondsSinceEpoch_)/1000000;
}

Timestamp Timestamp::addTime(Timestamp timestamp,double seconds) {
    return Timestamp(timestamp.microSecondsSinceEpoch() + seconds * 1000 * 1000);
}