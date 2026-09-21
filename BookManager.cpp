// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "BookManager.h"
#include "FileUtil.h"
#include <iostream>
#include <iomanip>

using namespace std;

BookManager::BookManager(const string& filePath) : path(filePath) {}

// 从 CSV 加载图书。每行 8 个字段：编号,书名,作者,出版社,ISBN,分类,总库存,可借数量
void BookManager::load() {
    for (const auto& row : readCSV(path)) {
        if (row.size() < 8) continue; // 字段不足的行跳过，防止越界
        Book b(toInt(row[0]), row[1], row[2], row[3], row[4], row[5],
               toInt(row[6]), toInt(row[7]));
        books.push_back(b);
    }
}

// 把所有图书写回 CSV
void BookManager::save() const {
    vector<vector<string>> rows;
    for (const auto& b : books) {
        rows.push_back(b.toCSV());
    }
    writeCSV(path, rows);
}

// 列出全部图书
void BookManager::listBooks() const {
    cout << "\n---------------- 图书列表 ----------------\n";
    cout << left
         << setw(6)  << "编号"
         << setw(20) << "书名"
         << setw(12) << "作者"
         << setw(12) << "分类"
         << setw(8)  << "可借/总\n";
    for (const auto& b : books) {
        b.printInfo();
    }
    cout << "------------------------------------------\n";
}

// 按关键字搜索：在编号/书名/作者/分类里做“包含”匹配
void BookManager::searchBooks(const string& keyword) const {
    cout << "\n---------------- 搜索结果 ----------------\n";
    bool found = false;
    for (const auto& b : books) {
        if (to_string(b.getId()).find(keyword) != string::npos ||
            b.getTitle().find(keyword) != string::npos ||
            b.getAuthor().find(keyword) != string::npos ||
            b.getCategory().find(keyword) != string::npos) {
            b.printInfo();
            found = true;
        }
    }
    if (!found) cout << "没有找到匹配的图书。\n";
    cout << "------------------------------------------\n";
}

// 按编号查找，返回指向该图书的指针；找不到返回 nullptr
Book* BookManager::findBook(int id) {
    for (auto& b : books) {
        if (b.getId() == id) return &b;
    }
    return nullptr;
}

// 下一个可用编号 = 当前最大编号 + 1（初始从 1001 开始）
int BookManager::getNextId() const {
    int maxId = 1000;
    for (const auto& b : books) {
        if (b.getId() > maxId) maxId = b.getId();
    }
    return maxId + 1;
}

// 交互式新增图书（编号自动分配）
void BookManager::addBook() {
    cout << "\n---------- 新增图书 ----------\n";
    int id = getNextId();
    string title = readLine("书名: ");
    string author = readLine("作者: ");
    string publisher = readLine("出版社: ");
    string isbn = readLine("ISBN: ");
    string category = readLine("分类: ");
    int total = readInt("总库存: ");
    // 新增时所有书都可借，所以可借数量 = 总库存
    books.push_back(Book(id, title, author, publisher, isbn, category, total, total));
    save();
    cout << "新增成功，图书编号为 " << id << "。\n";
}

// 直接添加一本图书（供初始化默认数据时使用，不读取键盘输入）
void BookManager::addBook(const Book& b) {
    books.push_back(b);
}

// 删除指定编号的图书
void BookManager::deleteBook(int id) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->getId() == id) {
            books.erase(it);
            save();
            cout << "删除成功。\n";
            return;
        }
    }
    cout << "未找到编号为 " << id << " 的图书。\n";
}

// 修改指定编号的图书（直接回车表示该项不修改）
void BookManager::updateBook(int id) {
    Book* b = findBook(id);
    if (!b) {
        cout << "未找到编号为 " << id << " 的图书。\n";
        return;
    }

    cout << "\n---------- 修改图书（直接回车表示不修改）----------\n";
    string t = readLine("新书名(" + b->getTitle() + "): ");
    if (!t.empty()) b->setTitle(t);

    string a = readLine("新作者(" + b->getAuthor() + "): ");
    if (!a.empty()) b->setAuthor(a);

    string p = readLine("新出版社(" + b->getPublisher() + "): ");
    if (!p.empty()) b->setPublisher(p);

    string i = readLine("新ISBN(" + b->getIsbn() + "): ");
    if (!i.empty()) b->setIsbn(i);

    string c = readLine("新分类(" + b->getCategory() + "): ");
    if (!c.empty()) b->setCategory(c);

    string totalStr = readLine("新总库存(" + to_string(b->getTotal()) + "): ");
    if (!totalStr.empty()) {
        int newTotal = toInt(totalStr, b->getTotal());
        // 可借数量随总库存变化同步调整：可借 += (新总 - 原总)
        int diff = newTotal - b->getTotal();
        b->setTotal(newTotal);
        b->setAvailable(b->getAvailable() + diff);
        if (b->getAvailable() < 0) b->setAvailable(0); // 不能为负
    }
    save();
    cout << "修改成功。\n";
}
