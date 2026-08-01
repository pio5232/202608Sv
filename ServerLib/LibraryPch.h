#pragma once

//#define LAN
//#define ECHO
#include <iostream>

#include <WinSock2.h>
#include <Windows.h>
#include <functional>
#include <string>
#include <array>
#include <thread>
#include <vector>

#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "pdh.lib")

#include "Define.h"
#include "Utils.h"
#include "GlobalInstance.h"

#include "NetworkUtils.h"

#include "IocpServer.h"
#include "IocpClient.h"

#include "MemorySystem.h"
#include "ObjectPool.h"

#include "Stopwatch.h"

#include "PacketDefine.h"
#include "Session.h"

#include "Logger.h"
#include "Profiler.h"
#include "Parser.h"

#include "JobQueue.h"
#include "Job.h"

#include "PerformanceDataInfo.h"
