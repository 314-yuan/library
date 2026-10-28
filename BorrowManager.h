#ifndef BORROWMANAGER_H
#define BORROWMANAGER_H

#include <string>
#include <vector>

class Reader;        // 前置声明，避免循环包含
class BookResource;  // 前置声明

// ============================================================
// 实验三：借阅 / 归还管理
//
// BorrowRecord 表示一条借阅记录；BorrowManager 用 vector 管理所有记录，
// 实现借书、还书、续借、逾期罚款、记录查询，并持久化到 CSV。
// ============================================================

// 借期与罚款规则（常量）
const int BORROW_DAYS = 30;       // 一次可借 30 天
const double FINE_PER_DAY = 0.5;  // 逾期每天罚款 0.5 元

// -------------------- BorrowRecord 借阅记录 --------------------
class BorrowRecord {
private:
    std::string username;   // 借阅人用户名
    int bookId;             // 资源编号
    std::string bookTitle;  // 题名（冗余存一份，方便资源删除后仍能显示）
    std::string bookType;   // 资源类型（冗余）
    std::string borrowDate; // 借阅日期 YYYY-MM-DD
    std::string dueDate;    // 应还日期 YYYY-MM-DD
    std::string returnDate; // 实际归还日期（空 = 尚未归还）

public:
    BorrowRecord(const std::string& username, int bookId,
                 const std::string& bookTitle, const std::string& bookType,
                 const std::string& borrowDate, const std::string& dueDate,
                 const std::string& returnDate = "");

    // getter
    const std::string& getUsername() const { return username; }
    int getBookId() const { return bookId; }
    const std::string& getBookTitle() const { return bookTitle; }
    const std::string& getBorrowDate() const { return borrowDate; }
    const std::string& getDueDate() const { return dueDate; }
    const std::string& getReturnDate() const { return returnDate; }
    bool isReturned() const { return !returnDate.empty(); } // 是否已归还

    void setReturnDate(const std::string& d) { returnDate = d; }
    void setDueDate(const std::string& d) { dueDate = d; }

    std::vector<std::string> toCSV() const; // 序列化为 CSV 一行
    void printInfo() const;                 // 打印单条记录
};

// -------------------- BorrowManager 借阅管理器 --------------------
class BorrowManager {
private:
    std::vector<BorrowRecord> records; // 所有借阅记录
    std::string path;                  // 数据文件路径（data/records.csv）

public:
    BorrowManager(const std::string& filePath);

    void load();       // 从 CSV 加载
    void save() const; // 写回 CSV

    // 借书：成功返回 true；失败（超上限/无可借/重复借）打印原因返回 false。
    // 内部会更新资源可借数量、读者已借数量，并保存 records.csv。
    bool borrow(Reader* reader, BookResource* resource);

    // 还书：成功返回罚款金额（0 表示未逾期）；失败返回 -1。
    // resource 可能为 nullptr（资源已被管理员删除），此时只更新记录和读者状态。
    double returnBook(Reader* reader, int bookId, BookResource* resource);

    // 续借：把应还日期再延长一个借期。成功返回 true。
    bool renew(Reader* reader, int bookId);

    // 查询
    void listUserRecords(const std::string& username) const; // 某读者的记录
    void listAllRecords() const;                             // 全部记录（管理员）
    void listOverdue() const;                                // 逾期记录（管理员）

private:
    // 找到某读者某资源的未还记录，返回指针；找不到返回 nullptr
    BorrowRecord* findUnreturned(const std::string& username, int bookId);
};

#endif
