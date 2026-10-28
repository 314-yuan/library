#ifndef BOOKRESOURCE_H
#define BOOKRESOURCE_H

#include <string>
#include <vector>

// ============================================================
// 实验三：图书资源继承层级（继承与派生）
//
// 抽象基类 BookResource（图书资源），派生类：
//   GeneralBook（一般图书）、Magazine（杂志）、EResource（电子资源）。
// 不同资源类型有各自的属性与显示方式，通过基类指针统一管理，
// 为实验四的虚函数多态打基础。
// ============================================================

// -------------------- 抽象基类 BookResource --------------------
class BookResource {
protected:
    int id;                 // 资源编号（唯一标识）
    std::string title;      // 题名
    std::string publisher;  // 出版社 / 来源
    int total;              // 总数量
    int available;          // 当前可借数量

public:
    BookResource(int id, const std::string& title, const std::string& publisher,
                 int total, int available);

    // 虚析构函数：保证通过基类指针 delete 派生类对象时能正确析构
    virtual ~BookResource() {}

    // ---------- getter / setter（公共属性，所有资源共有）----------
    int getId() const { return id; }
    const std::string& getTitle() const { return title; }
    const std::string& getPublisher() const { return publisher; }
    int getTotal() const { return total; }
    int getAvailable() const { return available; }
    void setTitle(const std::string& t) { title = t; }
    void setPublisher(const std::string& p) { publisher = p; }
    void setTotal(int t) { total = t; }
    void setAvailable(int a) { available = a; }

    // ---------- 借阅相关（所有资源通用的逻辑）----------
    bool canBorrow() const { return available > 0; }         // 是否还可借
    void borrowOne() { if (available > 0) --available; }     // 借出一本
    void returnOne() { if (available < total) ++available; } // 归还一本

    // ---------- 纯虚函数（= 0）----------
    // 不同资源类型“类型名、显示信息、CSV 序列化”都不同，故声明为纯虚，
    // 由各派生类分别实现。包含纯虚函数 => 抽象类，不能直接实例化。
    virtual std::string getType() const = 0;            // 返回资源类型名
    virtual void showInfo() const = 0;                  // 打印单条资源信息
    virtual std::vector<std::string> toCSV() const = 0; // 序列化为 CSV 一行

    // 工厂式创建：根据 CSV 一行字段，按第一列“类型”new 出对应派生类。
    // 类似 UserManager::createUser，体现“用基类指针统一管理多种对象”。
    static BookResource* createFromCSV(const std::vector<std::string>& f);
};

// -------------------- 派生类 GeneralBook（一般图书）--------------------
// 在公共属性基础上增加：作者、ISBN、分类。
class GeneralBook : public BookResource {
private:
    std::string author;   // 作者
    std::string isbn;     // ISBN 号
    std::string category; // 分类

public:
    GeneralBook(int id, const std::string& title, const std::string& publisher,
                int total, int available, const std::string& author,
                const std::string& isbn, const std::string& category);

    std::string getType() const override;
    void showInfo() const override;
    std::vector<std::string> toCSV() const override;

    // 一般图书特有属性
    const std::string& getAuthor() const { return author; }
    const std::string& getIsbn() const { return isbn; }
    const std::string& getCategory() const { return category; }
    void setAuthor(const std::string& a) { author = a; }
    void setIsbn(const std::string& i) { isbn = i; }
    void setCategory(const std::string& c) { category = c; }
};

// -------------------- 派生类 Magazine（杂志）--------------------
// 增加：期号、ISSN。
class Magazine : public BookResource {
private:
    std::string issue; // 期号（如 2026-10）
    std::string issn;  // ISSN 号

public:
    Magazine(int id, const std::string& title, const std::string& publisher,
             int total, int available, const std::string& issue,
             const std::string& issn);

    std::string getType() const override;
    void showInfo() const override;
    std::vector<std::string> toCSV() const override;

    const std::string& getIssue() const { return issue; }
    const std::string& getIssn() const { return issn; }
    void setIssue(const std::string& i) { issue = i; }
    void setIssn(const std::string& i) { issn = i; }
};

// -------------------- 派生类 EResource（电子资源）--------------------
// 增加：文件格式、下载地址/所在数据库。
class EResource : public BookResource {
private:
    std::string format; // 文件格式（PDF / EPUB / CAJ 等）
    std::string url;    // 下载地址或所在数据库

public:
    EResource(int id, const std::string& title, const std::string& publisher,
              int total, int available, const std::string& format,
              const std::string& url);

    std::string getType() const override;
    void showInfo() const override;
    std::vector<std::string> toCSV() const override;

    const std::string& getFormat() const { return format; }
    const std::string& getUrl() const { return url; }
    void setFormat(const std::string& f) { format = f; }
    void setUrl(const std::string& u) { url = u; }
};

#endif
