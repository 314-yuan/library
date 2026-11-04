// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "BookResource.h"
#include "FileUtil.h" // toInt
#include <iostream>
#include <iomanip>    // setw 设置列宽，用于对齐输出

using namespace std;

// ==================== 基类 BookResource ====================

BookResource::BookResource(int id, const string& title, const string& publisher,
                           int total, int available)
    : id(id), title(title), publisher(publisher),
      total(total), available(available) {}

// 工厂创建：CSV 第一列是类型标识，后续字段按类型解析。
// 各类型 CSV 列布局：
//   book      : book,id,title,publisher,total,available,author,isbn,category
//   magazine  : magazine,id,title,publisher,total,available,issue,issn
//   eresource : eresource,id,title,publisher,total,available,format,url
BookResource* BookResource::createFromCSV(const vector<string>& f) {
    if (f.size() < 6) return nullptr; // 至少要有公共的 6 列
    string type = f[0];
    int id = toInt(f[1]);
    string title = f[2];
    string publisher = f[3];
    int total = toInt(f[4]);
    int available = toInt(f[5]);

    if (type == "book") {
        return new GeneralBook(id, title, publisher, total, available,
            f.size() > 6 ? f[6] : "",  // 作者
            f.size() > 7 ? f[7] : "",  // ISBN
            f.size() > 8 ? f[8] : ""); // 分类
    } else if (type == "magazine") {
        return new Magazine(id, title, publisher, total, available,
            f.size() > 6 ? f[6] : "",  // 期号
            f.size() > 7 ? f[7] : ""); // ISSN
    } else if (type == "eresource") {
        // 电子资源无副本数量，不读 total/available，只取格式与下载地址
        return new EResource(id, title, publisher,
            f.size() > 6 ? f[6] : "",  // 格式
            f.size() > 7 ? f[7] : ""); // 下载地址
    }
    return nullptr; // 未知类型，跳过该行
}

// ==================== 派生类 GeneralBook ====================

GeneralBook::GeneralBook(int id, const string& title, const string& publisher,
                         int total, int available, const string& author,
                         const string& isbn, const string& category)
    : BookResource(id, title, publisher, total, available),
      author(author), isbn(isbn), category(category) {}

string GeneralBook::getType() const { return "一般图书"; }

void GeneralBook::showInfo() const {
    cout << left
         << setw(6)  << id
         << setw(10) << "一般图书"
         << setw(20) << title
         << setw(14) << author
         << setw(12) << category
         << setw(8)  << (to_string(available) + "/" + to_string(total)) << "\n";
}

vector<string> GeneralBook::toCSV() const {
    return { "book", to_string(id), title, publisher,
             to_string(total), to_string(available), author, isbn, category };
}

// ==================== 派生类 Magazine ====================

Magazine::Magazine(int id, const string& title, const string& publisher,
                   int total, int available, const string& issue,
                   const string& issn)
    : BookResource(id, title, publisher, total, available),
      issue(issue), issn(issn) {}

string Magazine::getType() const { return "杂志"; }

void Magazine::showInfo() const {
    cout << left
         << setw(6)  << id
         << setw(10) << "杂志"
         << setw(20) << title
         << setw(14) << issue
         << setw(12) << issn
         << setw(8)  << (to_string(available) + "/" + to_string(total)) << "\n";
}

vector<string> Magazine::toCSV() const {
    return { "magazine", to_string(id), title, publisher,
             to_string(total), to_string(available), issue, issn };
}

// ==================== 派生类 EResource ====================

// 电子资源不区分副本数量：total/available 对基类传 0 占位即可，
// 借阅判断与数量增减已由 canBorrow/borrowOne/returnOne 重写覆盖。
EResource::EResource(int id, const string& title, const string& publisher,
                     const string& format, const string& url)
    : BookResource(id, title, publisher, 0, 0),
      format(format), url(url) {}

string EResource::getType() const { return "电子资源"; }

void EResource::showInfo() const {
    cout << left
         << setw(6)  << id
         << setw(10) << "电子资源"
         << setw(20) << title
         << setw(14) << format
         << setw(12) << publisher
         << setw(8)  << "不限" << "\n"; // 电子资源无副本数量，显示“不限”
}

vector<string> EResource::toCSV() const {
    // 电子资源没有副本数量，total/available 两列输出 0 占位，
    // 仅为保持 CSV 列数与纸质资源一致（加载时会被解析为 0 并忽略）。
    return { "eresource", to_string(id), title, publisher,
             "0", "0", format, url };
}
