#ifndef BUFFER_H
#define BUFFER_H

#include<vector>
#include<string>

namespace ilib {
namespace net {
class Buffer
{
public:
    const static size_t kCheapPrepend = 0;
    const static size_t kInitialSize = 1024;

    Buffer();
    size_t readableBytes() const;
    size_t writableBytes() const;
    size_t prependableBytes() const;

    // 可读数据的起始地址
    const char * peek() const;

    void swap(Buffer & rhs);

    void retrieve(size_t len);

    void retrieveUntil(const char * end);

    void retrieveAll();

    std::string retrieveAsString();

    void append(const std::string & str);

    void hasWritten(size_t len);

    void ensureWritableBytes(size_t len);

    void append(const char * data,size_t len);

    void append(const void * data,size_t len);

    void prepend(const void *data,size_t len);

    ssize_t readFd(int , int * saveErrno);

    char * beginWrite();

private:
    char * begin();

    const char * begin() const;

    void makeSpace(size_t len);

    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};

}
}

#endif
