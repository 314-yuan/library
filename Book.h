#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

// ============================================================
// 实验二：Book 图书类
// 封装一本图书的属性，提供 getter/setter、CSV 序列化、打印功能。
// ============================================================
class Book {
private:
    int id;                 // 图书编号（唯一标识，自动生成）
    std::string title;      // 书名
    std::string author;     // 作者
    std::string publisher;  // 出版社
    std::string isbn;       // ISBN 号
    std::string category;   // 分类
    int total;              // 总库存
    int available;          // 当前可借数量

public:
    // 默认构造函数（给成员一个初始值）
    Book();

    // 带全部参数的构造函数
    Book(int id, const std::string& title, const std::string& author,
         const std::string& publisher, const std::string& isbn,
         const std::string& category, int total, int available);

    // ---------- getter（只读访问，返回 const 引用避免拷贝）----------
    int getId() const { return id; }
    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    const std::string& getPublisher() const { return publisher; }
    const std::string& getIsbn() const { return isbn; }
    const std::string& getCategory() const { return category; }
    int getTotal() const { return total; }
    int getAvailable() const { return available; }

    // ---------- setter（修改图书信息时用）----------
    // 注意：编号 id 是唯一标识，不允许修改，所以没有 setId。
    void setTitle(const std::string& t) { title = t; }
    void setAuthor(const std::string& a) { author = a; }
    void setPublisher(const std::string& p) { publisher = p; }
    void setIsbn(const std::string& i) { isbn = i; }
    void setCategory(const std::string& c) { category = c; }
    void setTotal(int t) { total = t; }
    void setAvailable(int a) { available = a; }

    // ---------- 其他方法 ----------
    std::vector<std::string> toCSV() const; // 序列化为 CSV 的一行字段
    void printInfo() const;                 // 打印单本图书信息（对齐显示）
};

#endif
