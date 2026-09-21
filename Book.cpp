// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "Book.h"
#include <iostream>
#include <iomanip> // setw 设置列宽，用于对齐输出

using namespace std;

// 默认构造函数：给基本类型成员一个初始值
Book::Book() : id(0), total(0), available(0) {}

// 带参构造函数：用初始化列表依次初始化所有成员
Book::Book(int id, const string& title, const string& author,
           const string& publisher, const string& isbn,
           const string& category, int total, int available)
    : id(id), title(title), author(author), publisher(publisher),
      isbn(isbn), category(category), total(total), available(available) {}

// 序列化：把图书转成 CSV 的 8 个字段（顺序要和 books.csv 表头一致）
vector<string> Book::toCSV() const {
    return { to_string(id), title, author, publisher, isbn, category,
             to_string(total), to_string(available) };
}

// 打印单本图书（左对齐、按列宽对齐；可借/总库存合并成一个字段显示）
void Book::printInfo() const {
    cout << left
         << setw(6)  << id
         << setw(20) << title
         << setw(12) << author
         << setw(12) << category
         << setw(8)  << (to_string(available) + "/" + to_string(total)) << "\n";
}
