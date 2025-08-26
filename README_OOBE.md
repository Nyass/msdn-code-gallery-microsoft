# OOBE 时间戳读取程序

## 功能说明
这个C++程序用于读取Windows注册表中的OOBE（Out-Of-Box Experience，开箱即用体验）完成时间戳。

程序会读取以下注册表路径：
```
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\OOBE\OOBECompleteTimestamp
```

并解析`OOBECompleteTimestamp`值的前12字节，按每2字节一组解析为：
- 年（字节0-1）
- 月（字节2-3）
- 日（字节4-5）
- 时（字节6-7）
- 分（字节8-9）
- 秒（字节10-11）

## 编译方法

### 使用Visual Studio命令行工具
```bash
cl /EHsc read_oobe_timestamp.cpp advapi32.lib
```

### 使用MinGW
```bash
g++ -o read_oobe_timestamp.exe read_oobe_timestamp.cpp -ladvapi32
```

### 使用Visual Studio IDE
1. 创建一个新的Win32控制台应用程序项目
2. 将代码复制到项目中
3. 编译并运行

## 使用方法

**重要**：该程序需要管理员权限才能读取注册表。

### 运行方式：
1. 以管理员身份打开命令提示符
2. 运行编译后的程序：
```bash
read_oobe_timestamp.exe
```

## 代码文件说明

- `read_oobe_timestamp.cpp` - 完整版本，包含详细的错误处理和调试信息
- `read_oobe_timestamp_method.cpp` - 简洁版本，只包含核心方法

## 注意事项

1. 程序使用Windows API，只能在Windows系统上编译和运行
2. 需要链接`advapi32.lib`库（包含注册表相关API）
3. 使用小端字节序解析数据（Windows默认）
4. 如果注册表键不存在或数据格式不正确，程序会返回错误

## 示例输出
```
OOBE完成时间:
年: 2024
月: 1
日: 15
时: 10
分: 30
秒: 45

格式化时间: 2024-01-15 10:30:45
```