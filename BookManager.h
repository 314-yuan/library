#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include <string>
#include <vector>
#include "BookResource.h"

// ============================================================
// 实验三：BookManager 图书资源管理器（由实验二的 Book 单类改造而来）
// 容器由 vector<Book> 改为 vector<BookResource*>（基类指针），
// 统一管理一般图书 / 杂志 / 电子资源三种对象。
// ============================================================
class BookManager {
private:
    std::vector<BookResource*> books; // 基类指针容器：可同时存放三种派生类对象
    std::string path;                 // 数据文件路径（data/books.csv）

public:
    BookManager(const std::string& filePath);
    ~BookManager(); // 负责释放所有 new 出来的对象

    // ---------- 文件读写（持久化）----------
    void load();       // 启动时从 CSV 加载（按类型字段工厂创建）
    void save() const; // 数据变化后写回 CSV（多态序列化）

    // ---------- 增删改查 ----------
    void addBook();                         // 交互式新增（先选资源类型）
    void addBook(BookResource* b);          // 直接添加（接管指针所有权，用于默认数据）
    void deleteBook(int id);                // 按编号删除
    void updateBook(int id);                // 交互式修改
    BookResource* findBook(int id);         // 按编号查找，返回基类指针
    void listBooks() const;                 // 列出全部资源
    void searchBooks(const std::string& keyword) const; // 按关键字搜索

    int getNextId() const;                  // 计算下一个可用编号（最大编号 + 1）
};

#endif
