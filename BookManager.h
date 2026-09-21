#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <string>
#include <vector>
#include "Book.h"

// ============================================================
// 实验二：BookManager 图书管理器
// 用 vector<Book> 容器管理所有图书，实现增删改查 + 文件持久化。
// ============================================================
class BookManager {
private:
    std::vector<Book> books; // vector 容器：存放所有图书对象
    std::string path;        // 数据文件路径（data/books.csv）

public:
    BookManager(const std::string& filePath);

    // ---------- 文件读写（持久化）----------
    void load();       // 启动时从 CSV 加载所有图书
    void save() const; // 数据变化后把所有图书写回 CSV

    // ---------- 增删改查 ----------
    void addBook();                        // 交互式新增（编号自动分配）
    void addBook(const Book& b);           // 直接添加一本（初始化默认数据用）
    void deleteBook(int id);               // 按编号删除
    void updateBook(int id);               // 交互式修改（回车表示不修改该项）
    Book* findBook(int id);                // 按编号查找，返回指针（找不到返回 nullptr）
    void listBooks() const;                // 列出全部图书
    void searchBooks(const std::string& keyword) const; // 按关键字搜索

    int getNextId() const;                 // 计算下一个可用编号（最大编号 + 1）
};

#endif
