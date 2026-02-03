# Example programs
The provided C++ example client programs are simple console applications for Windows. 

## Overview
The programs implement these functions:

The `ConsoleCloseHandler()` function uses the Windows API for allowing a clean shutdown of the network connection when the console is closed.

The `ReportError()` function relies on the API function `TargetAccessGetLastErrorMsg()`:
```c
size_t TargetAccessGetLastErrorMsg(char *errorMsg,
                                   size_t bufSize)
```
and is invoked upon a `DemoException` is catched to demonstrate how errors can be handled.

All the programs initiate the Target Access Client in the `main()` function:
```c
int TargetAccessInitializeWithPort(const char *serverName,
                                   int port)
```
using the default Target Access plugin port `9931/UDP`, except if the environment variable `TARGET_ACCESS_PORT` specifies otherwise.


## Accessing Target Memory
### `mem-reader`
The [`mem-reader.cpp`](mem-reader.cpp) example application demonstrates how to read from the target device memory. It uses the `TargetAccessReadTargetMemory()` function
```c
int TargetAccessReadTargetMemory(TargetAccessAddr address,
                                 unsigned char *buffer,
                                 uint16_t numBytes)
```
to read 4 bytes of memory at the `address`, entered as command line parameter. The `buffer` points to a string in the program.


### `mem-writer`
The [`mem-writer.cpp`](mem-writer.cpp) example application demonstrates how to write the target device memory. It uses the `TargetAccessWriteTargetMemory()` function 
```c
int TargetAccessWriteTargetMemory(TargetAccessAddr address,
                                  const unsigned char *buffer,
                                  uint16_t numBytes)
```
to write to a 32-bit `buffer` to the target's memory `address`. Both `address` and `buffer` are entered as command line parameters.


## ITM Events Listener
### `itm-logger`
The [`itm-logger.cpp`](itm-logger.cpp) example application implements a logger for ITM events. The most relevant code is in the `RunItmLogging()` function.

The `TargetAccessConfigureItmListener()` API function
```c
int TargetAccessConfigureItmListener(uint32_t channels,
                                     uint32_t bufferCapacity)
```
sets the bit mask for which ITM channels the client application will be listening to (`0xfff00000`). It also sets the buffer capacity for `1000` events.

Within the `while(1)` loop, the API function
```c
int TargetAccessReadItmEvent(ITMEvent *itmEvent,
                             bool *isValid)
```
polls for a new ITM event every `duration` seconds. The `ITMevent` block:
```c
struct ITMEvent
{
  uint8_t portId;
  uint64_t timeStamp;
  uint32_t dataLength;
  uint8_t data[4];
};
```
becomes invalid for the current iteration if the `isValid` argument is false and then `ReportITMStatus()` is executed.

The program runs indefinitely until `Ctrl-C` is pressed or the console is closed, as implemented in the `ConsoleCloseHandler()`.
