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


struct WaitEvent
{
	std::mutex mtx;
	std::unique_lock<std::mutex> locker;
	std::condition_variable cv;

	WaitEvent()
	{
		std::unique_lock<std::mutex> locker1(mtx);
		locker.swap(locker1);
		isWait = false;
	}

	bool IsWait() { return isWait; }

	void WaitResult() {
		//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
		cv.wait(locker);
		isWait = true;
	}
	void WaitResultMillionSecond(int ms) {
		//cv.wait_for(locker,std::chrono::duration_cast<std::chrono::seconds>(5));
		cv.wait_for(locker, std::chrono::milliseconds(ms));
		isWait = true;
	}
	void Notify()
	{
		isWait = false;
		cv.notify_one();
	}
private:
	bool isWait;
};