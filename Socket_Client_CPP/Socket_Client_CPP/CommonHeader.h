#pragma once

#ifndef _CommonHeader_H_
#define _CommonHeader_H_
#endif // !_CommonHeader_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
using namespace std;

#pragma warning(disable:4996)
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
