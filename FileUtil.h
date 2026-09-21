#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>

// ============================================================
// 实验二：文件工具模块
// 封装 CSV 文件的读写、字符串处理、日期计算、控制台输入等通用功能，
// 供 UserManager、BookManager（以及实验三的 BorrowManager）复用，
// 避免每个管理器重复写相同的代码。
// ============================================================

// ---------- 文件读写 ----------

// 判断文件是否存在
bool fileExists(const std::string& path);

// 按分隔符拆分字符串（如把 "a,b,c" 拆成 {"a","b","c"}）
std::vector<std::string> split(const std::string& s, char delim);

// 去除字符串首尾的空白字符（空格、制表符、换行等）
std::string trim(const std::string& s);

// 读取 CSV 文件，返回二维字符串表：外层是行，内层是字段。
// 文件不存在时返回空表，不会报错。
std::vector<std::vector<std::string>> readCSV(const std::string& path);

// 把二维字符串表写入 CSV 文件（覆盖写，每行一条记录）
void writeCSV(const std::string& path, const std::vector<std::vector<std::string>>& rows);

// ---------- 类型转换 ----------

// 字符串转 int；转换失败时返回默认值，避免文件数据异常导致程序崩溃
int toInt(const std::string& s, int defaultValue = 0);

// 字符串转 double；转换失败时返回默认值
double toDouble(const std::string& s, double defaultValue = 0.0);

// ---------- 日期工具（供实验三借阅/归还计算借期与逾期使用）----------

// 返回今天日期，格式 "YYYY-MM-DD"
std::string getToday();

// 日期加 N 天
std::string addDays(const std::string& date, int days);

// 两个日期相差的天数（返回 to - from）
int daysBetween(const std::string& from, const std::string& to);

// ---------- 控制台输入 ----------

// 从控制台读取一行字符串
std::string readLine(const std::string& prompt);

// 从控制台读取一个整数（带输入校验，防止输入非数字导致死循环）
int readInt(const std::string& prompt);

#endif
