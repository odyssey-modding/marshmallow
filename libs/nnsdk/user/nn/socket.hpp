/**
 * @file socket.h
 * @brief Functions for opening sockets for wireless communication.
 */

#pragma once

#include <nn/types.h>
#include <nn/util.h>

struct in_addr {
    u32 s_addr;
};
struct sockaddr_in {
    u8 sin_len;
    u8 sin_family;
    u16 sin_port;
    in_addr sin_addr;
    char sin_zero[8];
};

struct sockaddr {
    u8 sa_len;
    u8 sa_family;
    char sa_data[14];
};

struct hostent {
    char* h_name;
    char** h_aliases;
    int h_addrtype;
    int h_length;
    char** h_addr_list;
};

#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define TCP_NODELAY 1

namespace nn {
    namespace socket {

#if NN_SDK_VER >= NN_MAKE_VER(7, 0, 0)
        struct InAddr {
            u32 addr;
        };
#else
        using InAddr = ::in_addr;
#endif

        Result Initialize(void* pool, ulong poolSize, ulong allocPoolSize, int concurLimit);
        Result Finalize();
        s32 SetSockOpt(s32 socket, s32 socketLevel, s32 option, const void*, u32 len);
        u64 Send(s32 socket, const void* buffer, u64 bufferLength, s32 flags);
        s32 Socket(s32 domain, s32 type, s32 protocol);
        u16 InetHtons(u16);
        u32 InetAton(const char* addressStr, InAddr* addressOut);
        hostent* GetHostByName(const char* name);
        u32 Connect(s32 socket, const sockaddr* addr, u32 addrLen);
        Result Close(s32 socket);
        u32 Bind(s32 socket, const sockaddr* addr, u32 addrLen);
        u32 Listen(s32 socket, s32 backlog);
        u32 Accept(s32 socket, sockaddr* addrOut, u32* addrLenOut);
        s32 Recv(s32 socket, void* out, ulong outLen, s32 flags);
        u32 GetLastErrno();

    }  // namespace socket
}  // namespace nn
