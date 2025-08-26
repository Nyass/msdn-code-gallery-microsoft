#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>

struct OOBETimestamp {
    uint16_t year;
    uint16_t month;
    uint16_t day;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
};

class RegistryReader {
public:
    /**
     * 读取注册表OOBE完成时间戳并解析为结构化时间
     * @param timestamp 输出参数，解析后的时间戳结构
     * @return true 成功，false 失败
     */
    static bool ReadOOBECompleteTimestamp(OOBETimestamp& timestamp) {
        HKEY hKey;
        LONG result;
        
        // 打开注册表键
        result = RegOpenKeyExA(
            HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE",
            0,
            KEY_READ,
            &hKey
        );
        
        if (result != ERROR_SUCCESS) {
            std::cerr << "无法打开注册表键. 错误代码: " << result << std::endl;
            return false;
        }
        
        // 查询值的大小
        DWORD dataSize = 0;
        DWORD dataType = 0;
        
        result = RegQueryValueExA(
            hKey,
            "OOBECompleteTimestamp",
            nullptr,
            &dataType,
            nullptr,
            &dataSize
        );
        
        if (result != ERROR_SUCCESS) {
            std::cerr << "无法查询注册表值大小. 错误代码: " << result << std::endl;
            RegCloseKey(hKey);
            return false;
        }
        
        // 确保至少有12字节的数据
        if (dataSize < 12) {
            std::cerr << "数据大小不足12字节. 实际大小: " << dataSize << " 字节" << std::endl;
            RegCloseKey(hKey);
            return false;
        }
        
        // 读取注册表值
        std::vector<BYTE> buffer(dataSize);
        result = RegQueryValueExA(
            hKey,
            "OOBECompleteTimestamp",
            nullptr,
            &dataType,
            buffer.data(),
            &dataSize
        );
        
        RegCloseKey(hKey);
        
        if (result != ERROR_SUCCESS) {
            std::cerr << "无法读取注册表值. 错误代码: " << result << std::endl;
            return false;
        }
        
        // 解析前12字节，每两字节为一组
        // 假设数据格式为小端序存储的16位整数
        timestamp.year = *reinterpret_cast<uint16_t*>(&buffer[0]);
        timestamp.month = *reinterpret_cast<uint16_t*>(&buffer[2]);
        timestamp.day = *reinterpret_cast<uint16_t*>(&buffer[4]);
        timestamp.hour = *reinterpret_cast<uint16_t*>(&buffer[6]);
        timestamp.minute = *reinterpret_cast<uint16_t*>(&buffer[8]);
        timestamp.second = *reinterpret_cast<uint16_t*>(&buffer[10]);
        
        return true;
    }
    
    /**
     * 显示原始字节数据（用于调试）
     */
    static void DisplayRawBytes() {
        HKEY hKey;
        LONG result;
        
        result = RegOpenKeyExA(
            HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE",
            0,
            KEY_READ,
            &hKey
        );
        
        if (result != ERROR_SUCCESS) {
            std::cerr << "无法打开注册表键进行调试显示" << std::endl;
            return;
        }
        
        DWORD dataSize = 0;
        DWORD dataType = 0;
        
        result = RegQueryValueExA(hKey, "OOBECompleteTimestamp", nullptr, &dataType, nullptr, &dataSize);
        
        if (result == ERROR_SUCCESS && dataSize >= 12) {
            std::vector<BYTE> buffer(dataSize);
            result = RegQueryValueExA(hKey, "OOBECompleteTimestamp", nullptr, &dataType, buffer.data(), &dataSize);
            
            if (result == ERROR_SUCCESS) {
                std::cout << "原始字节数据 (前12字节):" << std::endl;
                for (int i = 0; i < 12 && i < static_cast<int>(dataSize); i += 2) {
                    std::cout << "字节 " << i << "-" << (i+1) << ": ";
                    std::cout << std::hex << std::setfill('0') << std::setw(2) 
                              << static_cast<int>(buffer[i]) << " ";
                    std::cout << std::hex << std::setfill('0') << std::setw(2) 
                              << static_cast<int>(buffer[i+1]) << " ";
                    std::cout << "(十进制: " << std::dec 
                              << *reinterpret_cast<uint16_t*>(&buffer[i]) << ")" << std::endl;
                }
            }
        }
        
        RegCloseKey(hKey);
    }
};

// 使用示例
int main() {
    std::cout << "正在读取OOBE完成时间戳..." << std::endl;
    
    // 显示原始字节数据用于调试
    RegistryReader::DisplayRawBytes();
    std::cout << std::endl;
    
    OOBETimestamp timestamp;
    
    if (RegistryReader::ReadOOBECompleteTimestamp(timestamp)) {
        std::cout << "OOBE完成时间戳解析成功:" << std::endl;
        std::cout << "年份: " << timestamp.year << std::endl;
        std::cout << "月份: " << timestamp.month << std::endl;
        std::cout << "日期: " << timestamp.day << std::endl;
        std::cout << "小时: " << timestamp.hour << std::endl;
        std::cout << "分钟: " << timestamp.minute << std::endl;
        std::cout << "秒数: " << timestamp.second << std::endl;
        
        // 格式化输出
        std::cout << std::endl << "格式化时间: ";
        std::cout << std::setfill('0') << std::setw(4) << timestamp.year << "-";
        std::cout << std::setfill('0') << std::setw(2) << timestamp.month << "-";
        std::cout << std::setfill('0') << std::setw(2) << timestamp.day << " ";
        std::cout << std::setfill('0') << std::setw(2) << timestamp.hour << ":";
        std::cout << std::setfill('0') << std::setw(2) << timestamp.minute << ":";
        std::cout << std::setfill('0') << std::setw(2) << timestamp.second << std::endl;
    } else {
        std::cout << "读取OOBE完成时间戳失败" << std::endl;
        std::cout << "请确保:" << std::endl;
        std::cout << "1. 程序以管理员权限运行" << std::endl;
        std::cout << "2. 注册表键存在" << std::endl;
        std::cout << "3. 系统已完成OOBE设置" << std::endl;
    }
    
    std::cout << std::endl << "按任意键退出...";
    std::cin.get();
    
    return 0;
}