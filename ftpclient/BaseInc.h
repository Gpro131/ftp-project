#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <functional>
#include <utility>
#include <stdlib.h>
#include <thread>
#include <WinSock2.h>
#include <vector>
#include <map>
#include <mutex>
#include <deque>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <functional>
#include <chrono>
#pragma comment(lib,"WS2_32.lib")

using namespace std;


#define SendSize 10480
#define RecvSize 10480