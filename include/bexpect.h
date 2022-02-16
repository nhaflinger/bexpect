#ifndef BEXPECT_H
#define BEXPECT_H

#include <time.h>
#include <errno.h> 
#include <vector>
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <math.h>
#include <pthread.h>
#include "bsocket.h"
#include "blang.h"


enum class BL_matchReply
{
    SUCCESS = 1,
    PARITY_ERROR = 2,
    DATA_OVERRUN = 3,
    UNKNOWN_ERROR = 4,
};

using namespace std;

struct connnection 
{
    std::string local_host;
    int local_port;
};

typedef struct 
{ 
    float wait_period;
    std::string send_packet;
    std::string expect_packet;
} ArgList;


class blang
{
public:
    blang() {};
    ~blang() {};

    int wait(long usec);
    bool expect(bsocket* socket, std::string packet);
    int send(bsocket* socket, std::string packet);

    string removeWhiteSpace(string src);
    void printHex(unsigned char *src, size_t n);
    void hextobin(unsigned char *dst, unsigned char *src, size_t n);

private:
};

/** public interfaces */

/** wait for usec microseconds */
int blang::wait(long usec)
{
    struct timespec rs;
    int result;

    if (usec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    rs.tv_sec = usec / 1000000;
    rs.tv_nsec = (usec % 1000000) * 1000000000;

    do {
        result = nanosleep(&rs, &rs);
    } while (result && errno == EINTR);

    return result;
}

/** send a short packet */
int blang::send(bsocket* socket, std::string packet)
{ 
    string src;
    src = removeWhiteSpace(packet);

    int byte_array_size = (src.length() + 1) / 2;
    unsigned char* dst = new unsigned char[byte_array_size];

    hextobin(dst, (unsigned char*)src.c_str(), byte_array_size);
    //printHex(dst, byte_array_size);
    int bytes_sent = socket->sendMessage((char*)dst);
    
    delete[] dst;

    return bytes_sent;
}

/** wait for hextstr match */
bool blang::expect(bsocket* socket, std::string packet)
{
    /** need to check for masking symbols */
    vector <string> tokens;
    stringstream check1(packet);
    string intermediate;     
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
     
    string message; 
    string mask; 
    for(int i = 0; i < tokens.size(); i++)
    {
        stringstream check2(tokens[i]);   
        vector <string> pairs; 
        while(getline(check2, intermediate, ':'))
        {
            pairs.push_back(intermediate);
        }
        
        if (pairs.size() > 1)
        {
            for(int j = 0; j  < pairs.size(); j+=2)
            {
                mask += pairs[j];
                message += pairs[j+1];
            }
        }
        else 
        {
            message += tokens[i];
        }
    }
    
    int bytes_received = socket->receiveMessage();
    char* readbuf = socket->readBuffer();

    // convert hex string to number
    int msk = stoi(mask, 0, 16);
    int msg = stoi(message, 0, 16);

    int match = (long)readbuf & msk;
    //cout << "DEBUG: " << msk << " " << match << endl;

    if (match == msg) 
    {
        return true;
    }

    return false;
}

/** print bytes to check conversion from hex */
void blang::printHex(unsigned char* src, size_t n) 
{
    int i;
    for (i=0; i<n; ++i)
    {
        printf("%02x", (unsigned int)src[i]);
    }
    printf("\n");
}

/** remove white space */
string blang::removeWhiteSpace(string src)
{
    stringstream ss(src);
    string temp_str;  
    string dst;
    
    while (getline(ss, temp_str, ' '))
    {
        if (temp_str.length() == 1)
        {
            temp_str = "0" + temp_str;
        }
        dst += temp_str;
    }
    return dst;
}

/** convert hex string to bytes */
void blang::hextobin(unsigned char* dst, unsigned char* src, size_t n) 
{
    int i;
    char t[3];
    unsigned char* p = src;
    for (i=0; i<n; ++i) 
    {
        memcpy(t, p, 2);
        t[2] = '\0';
        dst[i] = (int)strtol(t, NULL, 16);
        p += 2;
    }
}

#endif //BEXPECT_H