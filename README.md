# Windows OOBE 完成时间戳读取器

这个C++程序用于读取Windows注册表中的OOBE（Out-of-Box Experience）完成时间戳，并将其解析为可读的日期时间格式。

## 功能特性

- 读取注册表键 `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\OOBE\OOBECompleteTimestamp`
- 解析前12字节的时间戳数据
- 将每两字节解析为年、月、日、时、分、秒
- 提供详细的错误处理和调试信息
- 显示原始字节数据用于调试

## 编译方法

### 使用 Microsoft Visual Studio
```bash
cl /EHsc registry_reader.cpp
```

### 使用 MinGW-w64
```bash
g++ -std=c++11 registry_reader.cpp -o registry_reader.exe
```

### 使用 Clang
```bash
clang++ -std=c++11 registry_reader.cpp -o registry_reader.exe
```

## 使用方法

1. **以管理员权限运行程序**（推荐）
   ```bash
   # 右键点击命令提示符选择"以管理员身份运行"
   registry_reader.exe
   ```

2. **或者以普通用户运行**
   ```bash
   registry_reader.exe
   ```

## 输出示例

```
正在读取OOBE完成时间戳...

原始字节数据 (前12字节):
字节 0-1: e4 07 (十进制: 2020)
字节 2-3: 0c 00 (十进制: 12)
字节 4-5: 1f 00 (十进制: 31)
字节 6-7: 17 00 (十进制: 23)
字节 8-9: 3b 00 (十进制: 59)
字节 10-11: 3b 00 (十进制: 59)

OOBE完成时间戳解析成功:
年份: 2020
月份: 12
日期: 31
小时: 23
分钟: 59
秒数: 59

格式化时间: 2020-12-31 23:59:59
```

## 代码结构

### `OOBETimestamp` 结构体
存储解析后的时间戳组件：
- `year`: 年份 (uint16_t)
- `month`: 月份 (uint16_t) 
- `day`: 日期 (uint16_t)
- `hour`: 小时 (uint16_t)
- `minute`: 分钟 (uint16_t)
- `second`: 秒数 (uint16_t)

### `RegistryReader` 类

#### `ReadOOBECompleteTimestamp(OOBETimestamp& timestamp)`
主要方法，用于读取和解析OOBE时间戳：
- 打开注册表键
- 读取 `OOBECompleteTimestamp` 值
- 验证数据大小（至少12字节）
- 解析前12字节为时间组件
- 返回操作成功/失败状态

#### `DisplayRawBytes()`
调试方法，显示原始字节数据：
- 以十六进制和十进制格式显示每两字节
- 帮助理解数据格式和调试解析问题

## 注意事项

1. **管理员权限**: 虽然读取注册表通常不需要管理员权限，但某些系统配置可能需要提升权限。

2. **数据格式假设**: 代码假设时间戳以小端序格式存储的16位整数。如果实际格式不同，可能需要调整解析逻辑。

3. **错误处理**: 程序包含完整的错误处理，会显示具体的错误代码和描述。

4. **兼容性**: 此程序仅适用于Windows系统，需要Windows API支持。

## 可能的错误情况

- **注册表键不存在**: 系统可能尚未完成OOBE设置
- **权限不足**: 某些系统配置可能需要管理员权限
- **数据格式不匹配**: 时间戳格式可能与预期不同
- **数据大小不足**: 注册表值小于12字节

## 扩展功能

可以考虑添加的功能：
- 支持不同的时间戳格式
- 转换为标准的 `time_t` 格式
- 支持批量读取多个OOBE相关的注册表值
- 添加时间戳有效性验证
