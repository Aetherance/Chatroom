#include"Buffer.h"
#include<assert.h>
#include<sys/uio.h>
#include"logger.h"

using namespace ilib::net;

Buffer::Buffer()
    : buffer_(kCheapPrepend + kInitialSize),
      readIndex_(kCheapPrepend),
      writeIndex_(kCheapPrepend) 
{
    assert(readableBytes() == 0);
    assert(writableBytes() == kInitialSize);
    assert(prependableBytes() == kCheapPrepend);
}

size_t Buffer::readableBytes() const {
    return writeIndex_ - readIndex_;
}

size_t Buffer::writableBytes() const {
    return buffer_.size() - writeIndex_;
}

size_t Buffer::prependableBytes() const {
    return readIndex_;
}

char * Buffer::begin() {
    return &*buffer_.begin();
}

const char * Buffer::begin() const {
    return &*buffer_.begin();
}

const char * Buffer::peek() const {
    return begin() + readIndex_;
}

void Buffer::swap(Buffer & that) {
    buffer_.swap(that.buffer_);
    std::swap(readIndex_,that.readIndex_);
    std::swap(writeIndex_,that.writeIndex_);
}

void Buffer::retrieve(size_t len) {
    assert(len <= readableBytes());
    readIndex_ += len;
}

void Buffer::retrieveUntil(const char * end) {
    assert(peek() <= end);
    assert(end<= beginWrite());
    retrieve(end - peek());
}

void Buffer::retrieveAll() {
    readIndex_ = kCheapPrepend;
    writeIndex_ = kCheapPrepend;
}

std::string Buffer::retrieveAsString() {
    std::string str(peek(),readableBytes());
    retrieveAll();
    return str;
}

void Buffer::append(const char * data,size_t len) {
    ensureWritableBytes(len);
    std::copy(data,data + len,beginWrite());
    hasWritten(len);
}

void Buffer::append(const std::string & str) {
    append(str.data(),str.size());
}

void Buffer::append(const void * data,size_t len) {
    append(static_cast<const char *>(data),len);
}

void Buffer::hasWritten(size_t len) {
    writeIndex_ += len;
}

void Buffer::ensureWritableBytes(size_t len) {
    if(writableBytes() < len) {
        makeSpace(len);
    }
    assert(writableBytes() >= len);
}

void Buffer::prepend(const void * data ,size_t len) {
    assert(len <= prependableBytes());
    readIndex_ -= len;
    const char * _data = static_cast<const char *>(data);
    std::copy(_data,_data+len,begin() + readIndex_);
}

char * Buffer::beginWrite() {
    return begin() + writeIndex_;
}

void Buffer::makeSpace(size_t len) {
    if(writableBytes() + prependableBytes() < len + kCheapPrepend) {
        buffer_.resize(writeIndex_ + len);
    } else {
        assert(kCheapPrepend < readIndex_);
        size_t readable = readableBytes();
        std::copy(begin() + readIndex_, begin() + writeIndex_ , begin() + kCheapPrepend);
        readIndex_ = kCheapPrepend;
        writeIndex_ = readIndex_ + readable;
        assert(readable == readableBytes());
    }
}

ssize_t Buffer::readFd(int fd,int * saveErrno) {
    char extrabuf[65535];
    iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf) ? 2 : 1);
    const ssize_t n = ::readv(fd,vec,iovcnt);
    if(n < 0) {
        *saveErrno = errno;
    } else if(static_cast<size_t>(n) <= writable) {
        writeIndex_ += n;
    } else {
        writeIndex_ = buffer_.size();
        append(extrabuf, n-writable);
    }
    return n;
}