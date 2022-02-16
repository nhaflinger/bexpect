#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bexpect.h"

using namespace std;


// read buffer
static char* m_read_buf = NULL;

// map of commands with argument list
extern std::vector<std::pair<std::string, std::string>> m_command_list;


/** this will update first occurrence of command */
void updateCommands(string command, string value)
{
    vector<pair<string, string>>::iterator it;
    for (it = m_command_list.begin(); it != m_command_list.end(); ++it)
    {
        if ((*it).first == command)
        {
            (*it).second = value;
            //cout << "Updating command: " << (*it).first << " " << (*it).second << endl; 
            break;
        }
    }
}

void parseArgs(ArgList* args, int argc, char* argv[])
{
    /** parse command line */
    for (int i = 1; i < argc; ++i)
    {
        string str1(argv[i]);
        if (str1 == "-w")
        {
            args->wait_period = atof(argv[i+1]);
            updateCommands("wait", argv[i+1]);
        }
        else if(!strcmp(argv[i], "-s"))
        {
            args->send_packet = string(argv[i+1]);
            updateCommands("send", argv[i+1]);
        }
        else if(!strcmp(argv[i], "-e"))
        {
            args->expect_packet = string(argv[i+1]);
            updateCommands("expect", argv[i+1]);
        }
    }
}

void usage()
{
    std::cout << "Usage: " << "bexpect remotehost:remoteport cmdfile [options]\n" <<
        "Description: Programmed dialog using packets send and received over network sockets\n" << 
        "Options:\n" <<
        "    -s           send-packet\n" <<
        "    -e           expect-packet\n" <<
        "    -w           wait-period\n" << endl;
}

bool openSocketConnections(bsocket* connection)
{
    blang* bl = new blang();

    // run comands
    vector<pair<string, string>>::iterator it;
    for (it = m_command_list.begin(); it != m_command_list.end(); ++it)
    {
        if ((*it).first == "wait")
        {
            //cout << (*it).first << " " << atoi((*it).second.c_str()) << endl; 
            bl->wait(atoi((*it).second.c_str()));
        }
        else if ((*it).first == "send")
        {
            //cout << (*it).first << " " << (*it).second.c_str() << endl; 
            bl->send(connection, bl->removeWhiteSpace((*it).second).c_str());
        }
        else if ((*it).first == "expect")
        {
            //cout << (*it).first << " " << (*it).second.c_str() << endl; 
            bool match = bl->expect(connection, (*it).second.c_str());
            if (match) 
            {
                std::cout << "expected reply received" << std::endl;
                exit(0);
            }
        }
        else
        {
            cout << "ERROR: command unimplemented" << endl;
        }
    }

    return(true);
}

void* socketThread(void* args)
{
    /** open local socket connection (client) */
    bsocket* connection = new bsocket(((struct connnection*)args)->local_host, ((struct connnection*)args)->local_port, true);

    bool finished = false;
    while (1)
    {
        finished = openSocketConnections(connection);
        if (finished) break;
    }

    // cleanup
    connection->closeSocket();

    return NULL;
}

int main(int argc, char* argv[])
{
    /* read command line arguments */
    const char* program = argv[0];
    if (argc <= 2)
    {
        usage();
        exit(0);
    }

    // parse command line
    string str1(argv[1]);
    string local_host_port;
    local_host_port.assign(str1);

    string delimiter = ":";
    string local_host, local_port;
    local_host = local_host_port.substr(0, local_host_port.find(delimiter));
    local_port = local_host_port.substr(local_host_port.find(delimiter)+1, local_host_port.back());
    
    string str2(argv[2]);
    string cmd_file;
    cmd_file.assign(str2);

    char* fullpath = realpath(cmd_file.c_str(), NULL);

    // open a file handle to a particular file:
    FILE *fh = fopen(fullpath, "r");
    // make sure it's valid:
    if (!fh) 
    {
        fprintf(stdout, "I can't open bexpect command file!\n");
        return -1;
    }
    yyread(fh);

    // parse command line options
    ArgList* args = new ArgList;
    parseArgs(args, argc, argv);

    // open local socket connection  
    pthread_t tid1;
    connnection sock_info;
    sock_info.local_host = local_host;
    sock_info.local_port = atoi(local_port.c_str());
    pthread_create(&tid1, NULL, socketThread, (void *)&sock_info);
    pthread_join(tid1, NULL);

    pthread_exit(NULL);

    return 0;
}