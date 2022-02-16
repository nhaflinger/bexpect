// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "bsocket.h"

using namespace std;
  

void printHex(unsigned char* src, size_t n) 
{
    int i;
    for (i=0; i<n; ++i)
    {
        printf("%02x", (unsigned int)src[i]);
    }
    printf("\n");
}

 vector<char> spacecraft(time_t sample_time, float delay)
{
    float_t battery_temp = 8. + 4. * sin(10. * sample_time);
    float_t battery_voltage = 30. + 1. * sin(10. * sample_time);
    float_t detector_temp =  230. + 5. * sin(10. * sample_time);
    uint64_t payload_error_flag = 0;
    uint32_t payload_status = 1;

    uint64_t data_len = 18;
    vector<char> packed_bytes;
    packed_bytes.resize(18);
    int offset = 0;
    memcpy(&packed_bytes[offset], &battery_temp, 4);
    offset += 4;
    memcpy(&packed_bytes[offset], &battery_voltage, 4);
    offset += 4;
    memcpy(&packed_bytes[offset], &detector_temp, 4);
    offset += 4;
    memcpy(&packed_bytes[offset], &payload_error_flag, 4);
    offset += 4;
    memcpy(&packed_bytes[offset], &payload_status, 2);

    time_t download_time = sample_time + delay;
    uint32_t packet_type = 2;
    uint32_t before_quiet = 0;

    vector<char> telemetry;
    telemetry.resize(50);
    offset = 0;
    memcpy(&telemetry[offset], &sample_time, 8);
    offset += 8;
    memcpy(&telemetry[offset], &download_time, 8);
    offset += 8;
    memcpy(&telemetry[offset], &data_len, 8);
    offset += 8;
    memcpy(&telemetry[offset], &packet_type, 4);
    offset += 4;
    memcpy(&telemetry[offset], &before_quiet, 4);
    offset += 4;
    memcpy(&telemetry[offset], &packed_bytes, data_len);

    return telemetry;
}

int main(int argc, char* argv[]) 
{
    /* read command line arguments */
    const char* program = argv[0];
    if (argc <= 1)
    {
        cout << "Host and port not defined" << endl;
        exit(0);
    }

    /** parse command line */
    string str1(argv[1]);
    string local_host_port;
    local_host_port.assign(str1);

    string delimiter = ":";
    string local_host, local_port;
    local_host = local_host_port.substr(0, local_host_port.find(delimiter));
    local_port = local_host_port.substr(local_host_port.find(delimiter)+1, local_host_port.back());

    /** open local socket connection (client) */
    bsocket* connection = new bsocket(local_host, stoi(local_port), false);
      
    while (1) {
        int bytes_received = connection->receiveMessage();
        char* readbuf = connection->readBuffer();

        // debug print
        printf("Client : bytes received = %d ", bytes_received);
        printf("message =  ");
        printHex((unsigned char*)readbuf, bytes_received);
        printf("Client message received.\n");
      
        // gmt time
        time_t rawtime;
        struct tm* ptm;
        time(&rawtime); 
        ptm = gmtime(&rawtime);

        float delay = 1300.;
        vector<char> telemetry = spacecraft(rawtime, delay);

        int bytes_sent = connection->sendMessage(telemetry.data());

        printf("Response sent.\n"); 
    }

    /** cleanup */
    connection->closeSocket();
      
    return 0;
}