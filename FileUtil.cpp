// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "FileUtil.h"
#include <fstream>   // 文件流
#include <sstream>   // 字符串流（拆分用）
#include <ctime>     // 日期时间
#include <cstdio>    // sscanf
#include <iostream>

using namespace std;

// ==================== 文件读写 ====================

bool fileExists(const string& path) {
    ifstream f(path.c_str());
    return f.good();
}

vector<string> split(const string& s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

string trim(const string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) return ""; // 全是空白字符
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, last - first + 1);
}

vector<vector<string>> readCSV(const string& path) {
    vector<vector<string>> rows;
    ifstream in(path);
    if (!in) return rows; // 文件不存在，返回空表
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue; // 跳过空行
        vector<string> fields = split(line, ',');
        for (auto& f : fields) f = trim(f); // 去掉每个字段首尾空白
        rows.push_back(fields);
    }
    return rows;
}

void writeCSV(const string& path, const vector<vector<string>>& rows) {
    ofstream out(path);
    if (!out) {
        cerr << "错误：无法写入文件 " << path << endl;
        return;
    }
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) out << ",";
            out << row[i];
        }
        out << "\n";
    }
}

// ==================== 类型转换 ====================

int toInt(const string& s, int defaultValue) {
    try {
        return stoi(s);
    } catch (...) { // 转换失败（非数字、越界等）
        return defaultValue;
    }
}

double toDouble(const string& s, double defaultValue) {
    try {
        return stod(s);
    } catch (...) {
        return defaultValue;
    }
}

// ==================== 日期工具 ====================

// 把 "YYYY-MM-DD" 转成 time_t（从 1970 年起的秒数），便于做加减运算
static time_t toTime(const string& date) {
    int y = 0, m = 0, d = 0;
    if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return 0;
    tm t = {};
    t.tm_year = y - 1900; // tm 中年份从 1900 开始计数
    t.tm_mon = m - 1;     // tm 中月份从 0 开始计数
    t.tm_mday = d;
    return mktime(&t);
}

string getToday() {
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return string(buf);
}

string addDays(const string& date, int days) {
    time_t t = toTime(date);
    t += (time_t)days * 24 * 3600; // 加 N 天的秒数
    tm* lt = localtime(&t);
    char buf[16];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return string(buf);
}

int daysBetween(const string& from, const string& to) {
    return (int)((toTime(to) - toTime(from)) / (24 * 3600));
}

// ==================== 控制台输入 ====================

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

int readInt(const string& prompt) {
    cout << prompt;
    int n;
    while (!(cin >> n)) {        // 输入不是整数时
        cin.clear();             // 清除错误标志
        cin.ignore(10000, '\n'); // 丢弃缓冲区里的错误输入
        cout << "输入无效，请输入整数: ";
    }
    cin.ignore(10000, '\n');     // 丢弃换行符，避免影响后续 getline
    return n;
}
