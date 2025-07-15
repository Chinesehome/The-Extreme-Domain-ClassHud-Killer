#define _WIN32_WINNT 0x0600
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <psapi.h>
#include <fstream>

#pragma comment(lib, "psapi.lib")

using namespace std;

// 终止所有同名进程
void terminateAllInstances(const wstring& processName) {
    PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    int terminatedCount = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    terminatedCount++;
                }
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);

    if (terminatedCount > 0) {
        wcout << L"已终止 " << terminatedCount << L" 个进程实例" << endl;
    }
}

// 检查进程是否运行
bool isProcessRunning(const wstring& processName) {
    PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

// 启动进程
bool startProcess(const wstring& path) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {0};
    
    wchar_t* cmdLine = _wcsdup(path.c_str());
    bool success = CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 
                                 CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    free(cmdLine);
    
    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    
    return success;
}

// 获取进程路径
bool getProcessPath(const wstring& processName, wstring& path) {
    PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
                if (hProcess) {
                    wchar_t buf[MAX_PATH];
                    if (GetModuleFileNameExW(hProcess, NULL, buf, MAX_PATH)) {
                        path = buf;
                        found = true;
                    }
                    CloseHandle(hProcess);
                }
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

// 宽字符串转窄字符串
string wstringToString(const wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
    return str;
}

// 检查是否是用户手动点击启动
bool isUserManualStart() {
    // 创建临时文件作为标记
    const char* markerFile = "manual_start_marker.tmp";
    
    // 检查标记文件是否存在
    ifstream file(markerFile);
    bool isManual = file.good();
    file.close();
    
    // 删除标记文件
    if (isManual) {
        remove(markerFile);
    }
    
    return isManual;
}

// 设置用户手动启动标记
void setManualStartMarker() {
    ofstream file("manual_start_marker.tmp");
    if (file.is_open()) {
        file << "manual";
        file.close();
    }
}

int main() {
    wstring processName = L"studentmain.exe";
    wstring processPath;
    bool hasPath = false;
    bool isProcessStarted = false; // 进程是否已手动启动
    int checkInterval = 500;       // 检查间隔（毫秒）

    // 初始化：先记录路径
    cout << "=== 初始化：请先确保目标进程已运行 ===" << endl;
    while (!hasPath) {
        if (isProcessRunning(processName)) {
            hasPath = getProcessPath(processName, processPath);
            if (hasPath) {
                cout << "已成功记录进程路径: " << wstringToString(processPath) << endl;
                Sleep(1000);
                break;
            } else {
                cout << "获取路径失败，请关闭并重新运行目标进程..." << endl;
            }
        } else {
            cout << "未检测到进程，请先手动运行 " << wstringToString(processName) << " ..." << endl;
        }
        Sleep(1000);
    }

    // 检查是否是用户手动启动
    bool manualStart = isUserManualStart();
    if (manualStart) {
        isProcessStarted = true;
        cout << "=== 检测到手动启动，进入【进程运行中】模式 ===" << endl;
    } else {
        cout << "=== 程序自动启动，进入【监控模式】（持续终止进程） ===" << endl;
    }

    // 屏幕区域定义
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int areaSize = 30;
    int triggerAreaX = screenWidth - areaSize; // 右上角触发区域

    cout << "\n=== 进程控制程序 ===" << endl;
    cout << "操作说明：" << endl;
    cout << "- 点击右上角" << areaSize << "x" << areaSize << "区域：" << endl;
    cout << "  1. 若进程未启动：启动进程并停止自动终止" << endl;
    cout << "  2. 若进程已启动：终止进程并恢复自动终止" << endl;

    while (true) {
        system("cls");
        POINT mouse;
        GetCursorPos(&mouse);

        // 显示当前状态
        cout << "当前模式：" << (isProcessStarted ? "【进程运行中】（不自动终止）" : "【监控模式】（持续终止）") << endl;
        cout << "进程状态：" << (isProcessRunning(processName) ? "运行中" : "已停止") << endl;
        cout << "鼠标位置：X=" << mouse.x << ", Y=" << mouse.y << endl;

        // 右上角点击触发
        if (mouse.x >= triggerAreaX && mouse.y <= areaSize && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            if (isProcessStarted) {
                // 终止进程并恢复自动终止
                terminateAllInstances(processName);
                isProcessStarted = false;
                cout << "已终止进程，恢复自动终止模式" << endl;
            } else {
                // 启动进程并停止自动终止
                setManualStartMarker(); // 设置手动启动标记
                if (startProcess(processPath)) {
                    isProcessStarted = true;
                    cout << "已启动进程，停止自动终止" << endl;
                } else {
                    cout << "启动失败！路径：" << wstringToString(processPath) << endl;
                }
            }
            Sleep(500);
        }

        // 仅当进程未手动启动时，持续终止它
        if (!isProcessStarted && isProcessRunning(processName)) {
            terminateAllInstances(processName);
        }

        Sleep(checkInterval);
    }

    return 0;
}