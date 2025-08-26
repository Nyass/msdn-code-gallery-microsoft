#include <windows.h>
#include <iostream>
#include <iomanip>
#include <vector>

// 结构体用于存储解析后的时间信息
struct OOBETimestamp {
    WORD year;
    WORD month;
    WORD day;
    WORD hour;
    WORD minute;
    WORD second;
};

// 读取并解析OOBE完成时间戳
bool ReadOOBECompleteTimestamp(OOBETimestamp& timestamp) {
    HKEY hKey;
    LONG result;
    
    // 打开注册表键
    const LPCWSTR keyPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE\\OOBECompleteTimestamp";
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    
    if (result != ERROR_SUCCESS) {
        std::cerr << "无法打开注册表键，错误代码: " << result << std::endl;
        return false;
    }
    
    // 准备读取数据
    BYTE data[256];
    DWORD dataSize = sizeof(data);
    DWORD dataType;
    
    // 读取OOBECompleteTimestamp值
    result = RegQueryValueExW(hKey, L"OOBECompleteTimestamp", NULL, &dataType, data, &dataSize);
    
    RegCloseKey(hKey);
    
    if (result != ERROR_SUCCESS) {
        std::cerr << "无法读取注册表值，错误代码: " << result << std::endl;
        return false;
    }
    
    // 检查是否有足够的数据（至少12字节）
    if (dataSize < 12) {
        std::cerr << "数据大小不足，需要至少12字节，实际: " << dataSize << std::endl;
        return false;
    }
    
    // 解析前12字节，每2字节为一组
    // 注意：Windows注册表通常使用小端字节序
    timestamp.year   = *reinterpret_cast<WORD*>(&data[0]);
    timestamp.month  = *reinterpret_cast<WORD*>(&data[2]);
    timestamp.day    = *reinterpret_cast<WORD*>(&data[4]);
    timestamp.hour   = *reinterpret_cast<WORD*>(&data[6]);
    timestamp.minute = *reinterpret_cast<WORD*>(&data[8]);
    timestamp.second = *reinterpret_cast<WORD*>(&data[10]);
    
    return true;
}

// 显示时间戳信息
void DisplayTimestamp(const OOBETimestamp& timestamp) {
    std::cout << "OOBE完成时间:" << std::endl;
    std::cout << "年: " << timestamp.year << std::endl;
    std::cout << "月: " << timestamp.month << std::endl;
    std::cout << "日: " << timestamp.day << std::endl;
    std::cout << "时: " << timestamp.hour << std::endl;
    std::cout << "分: " << timestamp.minute << std::endl;
    std::cout << "秒: " << timestamp.second << std::endl;
    
    // 格式化输出
    std::cout << "\n格式化时间: " 
              << std::setfill('0') 
              << std::setw(4) << timestamp.year << "-"
              << std::setw(2) << timestamp.month << "-"
              << std::setw(2) << timestamp.day << " "
              << std::setw(2) << timestamp.hour << ":"
              << std::setw(2) << timestamp.minute << ":"
              << std::setw(2) << timestamp.second << std::endl;
}

// 主函数示例
int main() {
    OOBETimestamp timestamp;
    
    if (ReadOOBECompleteTimestamp(timestamp)) {
        DisplayTimestamp(timestamp);
        
        // 显示原始字节数据（用于调试）
        std::cout << "\n原始字节数据（前12字节）:" << std::endl;
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
                         L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE\\OOBECompleteTimestamp", 
                         0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            BYTE data[12];
            DWORD dataSize = sizeof(data);
            DWORD dataType;
            
            if (RegQueryValueExW(hKey, L"OOBECompleteTimestamp", NULL, &dataType, data, &dataSize) == ERROR_SUCCESS) {
                for (int i = 0; i < 12; i++) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') 
                             << static_cast<int>(data[i]) << " ";
                    if ((i + 1) % 2 == 0) std::cout << "| ";
                }
                std::cout << std::dec << std::endl;
            }
            RegCloseKey(hKey);
        }
    } else {
        std::cerr << "读取OOBE时间戳失败！" << std::endl;
        std::cerr << "请确保以管理员权限运行程序。" << std::endl;
        return 1;
    }
    
    return 0;
}