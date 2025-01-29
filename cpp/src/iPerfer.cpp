#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

// Constants from the assignment
static const int ONE_BYTE_SIZE = 1;
static const size_t CHUNK_SIZE = 80000; // 80KB
static const int BACKLOG = 5;
static const int RTT_EXCHANGES = 8; // client does 8 round trips; server measures 7

bool sendAll(int sockfd, const char* buffer, size_t len)
{
    size_t totalSent = 0;
    while (totalSent < len)
    {
        ssize_t sent = send(sockfd, buffer + totalSent, len - totalSent, 0);
        if (sent < 0)
        {
            spdlog::error("send() failed: {}", strerror(errno));
            return false;
        }
        if (sent == 0)
        {
            // Connection closed unexpectedly
            return false;
        }
        totalSent += sent;
    }
    return true;
}

bool recvAll(int sockfd, char* buffer, size_t len)
{
    size_t totalRecv = 0;
    while (totalRecv < len)
    {
        ssize_t r = recv(sockfd, buffer + totalRecv, len - totalRecv, 0);
        if (r < 0)
        {
            spdlog::error("recv() failed: {}", strerror(errno));
            return false;
        }
        if (r == 0)
        {
            // Connection closed
            return false;
        }
        totalRecv += r;
    }
    return true;
}

// ===============================================================
// SERVER MODE 
// ===============================================================
void runServer(unsigned short port)
{
    // 1) Create socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
    {
        spdlog::error("Error creating server socket: {}", strerror(errno));
        exit(1);
    }

    // 2) Reuse address
    int optval = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 3) Bind
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(port);

    if (bind(serverSock, reinterpret_cast<sockaddr*>(&serverAddr),
             sizeof(serverAddr)) < 0)
    {
        spdlog::error("Error binding to port {}: {}", port, strerror(errno));
        close(serverSock);
        exit(1);
    }

    // 4) Listen
    if (listen(serverSock, BACKLOG) < 0)
    {
        spdlog::error("Error listening on socket: {}", strerror(errno));
        close(serverSock);
        exit(1);
    }
    spdlog::info("iPerfer server started");

    // 5) Accept
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSock = accept(serverSock,
                            reinterpret_cast<sockaddr*>(&clientAddr),
                            &clientLen);
    if (clientSock < 0)
    {
        spdlog::error("Error accepting connection: {}", strerror(errno));
        close(serverSock);
        exit(1);
    }
    spdlog::info("Client connected");

    close(serverSock);

    // 6) RTT measurement phase
    std::vector<double> rttSamples; 
    rttSamples.reserve(RTT_EXCHANGES - 1);

    char inByte = 0;
    auto ackSendTime      = std::chrono::high_resolution_clock::now();
    bool haveLastAckTime  = false;

    for (int i = 0; i < RTT_EXCHANGES; i++)
    {
        // Receive 1 byte from client
        if (!recvAll(clientSock, &inByte, ONE_BYTE_SIZE))
        {
            spdlog::error("RTT measurement: recv() failed");
            close(clientSock);
            return;
        }

        // If we have a prior ackSendTime, measure RTT
        if (haveLastAckTime)
        {
            auto now  = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(now - ackSendTime).count();
            rttSamples.push_back(ms);
        }

        // Send back 'A'
        char ack = 'A';
        if (!sendAll(clientSock, &ack, ONE_BYTE_SIZE))
        {
            spdlog::error("RTT measurement: send() failed");
            close(clientSock);
            return;
        }

        ackSendTime     = std::chrono::high_resolution_clock::now();
        haveLastAckTime = true;
    }

    // Compute average RTT from last 4
    double avgRTT = 0.0;
    int n = static_cast<int>(rttSamples.size()); // ~7
    if (n > 0)
    {
        int startIndex = (n > 4) ? (n - 4) : 0;
        double sum = 0.0;
        for (int i = startIndex; i < n; i++)
        {
            sum += rttSamples[i];
        }
        int count = n - startIndex;
        avgRTT = sum / count;
    }
    int   rttMillis = static_cast<int>(std::round(avgRTT));
    double avgRTTsec = avgRTT / 1000.0; // for net time calculation

    // 7) Data transfer phase
    auto dataStart = std::chrono::high_resolution_clock::now();

    long long totalBytesReceived = 0;
    std::vector<char> dataBuf(CHUNK_SIZE, '\0');
    int chunkCount = 0; // how many 80KB chunks are received

    while (true)
    {
        // We'll try to receive exactly 80KB
        if (!recvAll(clientSock, dataBuf.data(), CHUNK_SIZE))
        {
            // closed or error
            break;
        }
        totalBytesReceived += CHUNK_SIZE;
        chunkCount++;

        // Send 1-byte ack
        char ack = 'A';
        if (!sendAll(clientSock, &ack, ONE_BYTE_SIZE))
        {
            spdlog::error("Data transfer: ack send failed");
            break;
        }
    }

    auto dataEnd = std::chrono::high_resolution_clock::now();
    close(clientSock);

    double dataSeconds = std::chrono::duration<double>(dataEnd - dataStart).count();

    // 8) Subtract total RTT overhead
    // Because each 80KB chunk has a "stop-and-wait" for an ACK on the client side,
    // from the server's perspective we can do the same assumption: each chunk cost ~1 RTT
    double netSeconds = dataSeconds - (chunkCount * avgRTTsec);
    if (netSeconds < 0.0)
    {
        // fallback if negative
        netSeconds = dataSeconds;
    }

    // 9) Compute throughput using netSeconds
    double rateMbps = 0.0;
    if (netSeconds > 0.0)
    {
        rateMbps = (static_cast<double>(totalBytesReceived) * 8.0 / netSeconds) / 1e6;
    }
    long long totalKB = totalBytesReceived / 1000LL;

    // 10) Log final summary
    spdlog::info("Received={} KB, Rate={:.3f} Mbps, RTT={}ms",
                 totalKB, rateMbps, rttMillis);
}


// ===============================================================
// CLIENT MODE (MODIFIED to subtract total RTT overhead)
// ===============================================================
void runClient(const std::string& hostname, unsigned short port, double durationSeconds)
{
    // 1) Resolve hostname
    addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res);
    if (status != 0)
    {
        spdlog::error("getaddrinfo() failed: {}", gai_strerror(status));
        exit(1);
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(port);

    auto* ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
    serverAddr.sin_addr = ipv4->sin_addr;
    freeaddrinfo(res);

    // 2) Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        spdlog::error("Error creating client socket: {}", strerror(errno));
        exit(1);
    }

    // 3) Connect
    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
    {
        spdlog::error("Could not connect to {}:{} -> {}",
                      hostname, port, strerror(errno));
        close(sockfd);
        exit(1);
    }

    // 4) RTT measurement (8 times)
    std::vector<double> rttSamples;
    rttSamples.reserve(RTT_EXCHANGES);

    for (int i = 0; i < RTT_EXCHANGES; i++)
    {
        auto sendTime = std::chrono::high_resolution_clock::now();

        char outByte = 'M';
        if (!sendAll(sockfd, &outByte, ONE_BYTE_SIZE))
        {
            spdlog::error("RTT measurement: send() failed");
            close(sockfd);
            return;
        }

        char inByte = 0;
        if (!recvAll(sockfd, &inByte, ONE_BYTE_SIZE))
        {
            spdlog::error("RTT measurement: recv() failed");
            close(sockfd);
            return;
        }
        auto recvTime = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(recvTime - sendTime).count();
        rttSamples.push_back(ms);
    }

    // Average of last 4 RTT measurements
    double avgRTT = 0.0;
    int n = static_cast<int>(rttSamples.size()); // should be 8
    if (n > 0)
    {
        int startIndex = (n > 4) ? (n - 4) : 0;
        double sum = 0.0;
        for (int i = startIndex; i < n; i++)
        {
            sum += rttSamples[i];
        }
        int count = n - startIndex;
        avgRTT = sum / count;
    }
    // We'll keep the integer ms
    int rttMillis = static_cast<int>(std::round(avgRTT));
    // But also store it in seconds for the throughput correction
    double avgRTTsec = avgRTT / 1000.0; // convert ms -> sec

    // 5) Data transfer for <durationSeconds>, stop-and-wait
    std::vector<char> chunk(CHUNK_SIZE, '\0'); // 80KB of zeros
    long long totalBytesSent = 0;
    int chunkCount = 0; // how many 80KB chunks we send

    auto dataStart = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - dataStart).count();
        if (elapsed >= durationSeconds)
        {
            break;
        }

        // Send 80KB
        if (!sendAll(sockfd, chunk.data(), CHUNK_SIZE))
        {
            spdlog::error("Data transfer: send() failed");
            break;
        }
        totalBytesSent += CHUNK_SIZE;
        chunkCount++;

        // Wait for 1-byte ack
        char ackBuf[ONE_BYTE_SIZE];
        if (!recvAll(sockfd, ackBuf, ONE_BYTE_SIZE))
        {
            spdlog::error("Data transfer: ack receive failed (server closed?)");
            break;
        }
    }
    auto dataEnd = std::chrono::high_resolution_clock::now();

    close(sockfd);

    // 6) Calculate throughput
    double dataSeconds = std::chrono::duration<double>(dataEnd - dataStart).count();

    // We now subtract the "RTT overhead" from dataSeconds
    //   Because each chunk is stop-and-wait, we approximate that
    //   each chunk costs about 1 average RTT of "waiting" time.
    //   => total overhead ~ chunkCount * avgRTTsec
    double netSeconds = dataSeconds - (chunkCount * avgRTTsec);
    if (netSeconds < 0.0)
    {
        // Just in case it's negative from rounding or very large RTT
        netSeconds = dataSeconds;
    }

    // Then we compute throughput using netSeconds
    double rateMbps = 0.0;
    if (netSeconds > 0.0)
    {
        rateMbps = (static_cast<double>(totalBytesSent) * 8.0 / netSeconds) / 1e6;
    }
    long long totalKB = totalBytesSent / 1000LL;

    spdlog::info("Sent={} KB, Rate={:.3f} Mbps, RTT={}ms",
                 totalKB, rateMbps, rttMillis);
}

// ===============================================================
// MAIN - parse arguments, run server or client
// ===============================================================
int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        spdlog::error("Error: missing or extra arguments");
        return 1;
    }

    std::string mode(argv[1]);

    if ((mode == "-s" && argc != 4) || (mode == "-c" && argc != 8))
    {
        spdlog::error("Error: missing or extra arguments");
        return 1;
    }

    try
    {
        cxxopts::Options options("iPerfer", "Custom iPerf-like tool for EECS489");
        options.add_options()
            ("s,server", "Run in server mode")
            ("c,client", "Run in client mode")
            ("h,host", "Server hostname", cxxopts::value<std::string>())
            ("p,port", "Port number (1024 <= port <= 65535)", cxxopts::value<int>())
            ("t,time", "Duration in seconds (must be > 0)", cxxopts::value<double>())
            ("help", "Print help");

        auto parsed = options.parse(argc, argv);

        if (parsed.count("help") ||
           (!parsed.count("server") && !parsed.count("client")))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        bool isServer = parsed["server"].as<bool>();
        bool isClient = parsed["client"].as<bool>();

        if (!parsed.count("port"))
        {
            spdlog::error("Error: missing required port argument.");
            return 1;
        }
        int port = parsed["port"].as<int>();
        if (port < 1024 || port > 65535)
        {
            spdlog::error("Error: port number must be in the range of [1024, 65535]");
            return 1;
        }

        if (isServer)
        {
            if (parsed.count("host") || parsed.count("time"))
            {
                spdlog::error("Error: extra arguments provided in server mode.");
                return 1;
            }
            runServer(static_cast<unsigned short>(port));
        }
        else if (isClient)
        {
            if (!parsed.count("host") || !parsed.count("time"))
            {
                spdlog::error("Error: missing required -h <host> or -t <time> arguments.");
                return 1;
            }
            std::string hostname = parsed["host"].as<std::string>();
            double duration = parsed["time"].as<double>();
            if (duration <= 0.0)
            {
                spdlog::error("Error: time argument must be greater than 0");
                return 1;
            }
            runClient(hostname, static_cast<unsigned short>(port), duration);
        }
    }
    catch (const std::exception& e)
    {
        spdlog::error("Error parsing options: {}", e.what());
        return 1;
    }

    return 0;
}
