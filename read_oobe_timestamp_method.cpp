#include <windows.h>
#include <iostream>
#include <vector>

// 时间戳结构体
struct OOBETimestamp {
    WORD year;
    WORD month;
    WORD day;
    WORD hour;
    WORD minute;
    WORD second;
};

/**
 * 读取注册表HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\OOBE\OOBECompleteTimestamp
 * 键的OOBECompleteTimestamp值，并解析前12字节为时间信息
 * 
 * @param timestamp 输出参数，存储解析后的时间信息
 * @return 成功返回true，失败返回false
 */
bool ReadOOBECompleteTimestamp(OOBETimestamp& timestamp) {
    HKEY hKey;
    LONG result;
    
    // 打开注册表键
    const LPCWSTR keyPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE\\OOBECompleteTimestamp";
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        return false;
    }
    
    // 准备读取数据
    BYTE data[256];
    DWORD dataSize = sizeof(data);
    DWORD dataType;
    
    // 读取OOBECompleteTimestamp值
    result = RegQueryValueExW(hKey, L"OOBECompleteTimestamp", NULL, &dataType, data, &dataSize);
    
    RegCloseKey(hKey);
    
    if (result != ERROR_SUCCESS || dataSize < 12) {
        return false;
    }
    
    // 解析前12字节，每2字节为一组
    // 使用小端字节序（Windows默认）
    timestamp.year   = *reinterpret_cast<WORD*>(&data[0]);
    timestamp.month  = *reinterpret_cast<WORD*>(&data[2]);
    timestamp.day    = *reinterpret_cast<WORD*>(&data[4]);
    timestamp.hour   = *reinterpret_cast<WORD*>(&data[6]);
    timestamp.minute = *reinterpret_cast<WORD*>(&data[8]);
    timestamp.second = *reinterpret_cast<WORD*>(&data[10]);
    
    return true;
}

// 使用示例
int main() {
    OOBETimestamp timestamp;
    
    if (ReadOOBECompleteTimestamp(timestamp)) {
        std::cout << "OOBE完成时间: "
                  << timestamp.year << "-"
                  << timestamp.month << "-"
                  << timestamp.day << " "
                  << timestamp.hour << ":"
                  << timestamp.minute << ":"
                  << timestamp.second << std::endl;
    } else {
        std::cerr << "读取失败，请确保以管理员权限运行" << std::endl;
    }
    
    return 0;
}