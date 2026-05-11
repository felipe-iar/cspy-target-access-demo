/**************************************************
 *
 * Memory writer example (mem-writer.cpp)
 *
 * Copyright (c) 2023-2026 IAR Systems AB.
 *
 * See LICENSE for detailed license information.
 *
 **************************************************/

#include "TargetAccessClientSDK.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
BOOL WINAPI ConsoleCloseHandler(DWORD dwCtrlType)
{
  switch (dwCtrlType)
  {
    case CTRL_C_EVENT:
      std::cout << "Ctrl-C detected." << std::setfill(' ') << 
        std::setw(50) << std::endl;
      ::TargetAccessShutdown();
      return TRUE;
      
    case CTRL_CLOSE_EVENT:
      std::cout << "Target Access Client is shutting down" << 
        std::setfill(' ') << std::setw(50) << "" << std::endl;
      ::TargetAccessShutdown();
      return TRUE;
      
    default:
      return FALSE;
  }
}
#endif

class DemoException : public std::exception
{
public:
  DemoException()
  {
  }
};

void ReportError()
{
  std::string errorMsg;
  
  errorMsg.resize(512);
  ::TargetAccessGetLastErrorMsg(const_cast<char *>(errorMsg.data()), errorMsg.size());
  std::cout << "Server protocol version: " << ::TargetAccessGetServerProtocolVersion() << std::endl;
  std::cout << "Client protocol version: " << ::TargetAccessGetClientProtocolVersion() << std::endl;
  std::cout << "Error: " << errorMsg << std::endl;
}

int main(int argc, const char **argv)
{
#if defined(_WIN32) || defined(_WIN64)
  ::SetConsoleCtrlHandler(ConsoleCloseHandler, TRUE);
#endif

#if defined(_WIN32) || defined(_WIN64)
  char* buf = nullptr;
  size_t len;
  _dupenv_s(&buf, &len, "TARGET_ACCESS_PORT");
  int port = buf ? std::stoi(buf) : 9931; // TAP server default UDP port
#else
  const char* buf = getenv("TARGET_ACCESS_PORT");
  int port = buf ? std::stoi(buf) : 9931; // TAP server default UDP port
#endif
  try
  {
    if (argc < 3)
    {
      std::cout << "Usage: mem-writer <32-bit address> <32-bit value>" << std::endl;
      return 1;
    }
    TargetAccessAddr addr;
    std::istringstream astr(argv[1]);
    astr >> std::hex >> addr;
    
    union {
      uint32_t value;
      uint8_t buffer[4];
    } data;

    std::istringstream vstr(argv[2]);
    vstr >> std::hex >> data.value;

    // Initialize client
    if (::TargetAccessInitializeWithPort("localhost", port) != TARGET_ACCESS_STATUS_OK)
    {
      throw DemoException();
      ::TargetAccessShutdown();
      return 1;
    }
    for (size_t i = 0; i < 4; i++)
    {
      if (::TargetAccessWriteTargetMemory(addr, data.buffer, sizeof(data.buffer)) != TARGET_ACCESS_STATUS_OK)
      {
        throw DemoException();
        ::TargetAccessShutdown();
        return 1;
      }
    }
    std::cout << "Wrote: 0x" << std::hex << data.value << " @ 0x" << std::hex << addr << std::endl;
  }
  catch (const DemoException &)
  {
    ::ReportError();
  }
  ::TargetAccessShutdown();
  return 0;
}
