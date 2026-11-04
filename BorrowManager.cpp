// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "BorrowManager.h"
#include "User.h"          // Reader
#include "BookResource.h"  // BookResource
#include "FileUtil.h"      // readCSV / writeCSV / 日期工具 / toInt
#include "TemplateUtil.h"  // 实验五：通用模板函数 findValue
#include <iostream>
#include <iomanip>

using namespace std;

// ==================== BorrowRecord ====================

BorrowRecord::BorrowRecord(const string& username, int bookId,
                           const string& bookTitle, const string& bookType,
                           const string& borrowDate, const string& dueDate,
                           const string& returnDate)
    : username(username), bookId(bookId), bookTitle(bookTitle), bookType(bookType),
      borrowDate(borrowDate), dueDate(dueDate), returnDate(returnDate) {}

vector<string> BorrowRecord::toCSV() const {
    return { username, to_string(bookId), bookTitle, bookType,
             borrowDate, dueDate, returnDate };
}

void BorrowRecord::printInfo() const {
    cout << left
         << setw(10) << username
         << setw(6)  << bookId
         << setw(20) << bookTitle
         << setw(10) << bookType
         << setw(12) << borrowDate
         << setw(12) << dueDate
         << setw(12) << (returnDate.empty() ? "未还" : returnDate) << "\n";
}

// ==================== BorrowManager ====================

BorrowManager::BorrowManager(const string& filePath) : path(filePath) {}

void BorrowManager::load() {
    for (const auto& row : readCSV(path)) {
        // 字段：用户名,资源编号,题名,类型,借阅日期,应还日期,归还日期
        if (row.size() < 6) continue;
        string returnDate = row.size() > 6 ? row[6] : "";
        records.push_back(BorrowRecord(row[0], toInt(row[1]), row[2], row[3],
                                       row[4], row[5], returnDate));
    }
}

void BorrowManager::save() const {
    vector<vector<string>> rows;
    for (const auto& r : records) rows.push_back(r.toCSV());
    writeCSV(path, rows);
}

BorrowRecord* BorrowManager::findUnreturned(const string& username, int bookId) {
    // findValue 在值容器 vector<BorrowRecord> 中查找第一条未归还记录
    return findValue(records, [&](BorrowRecord& r) {
        return !r.isReturned() && r.getUsername() == username && r.getBookId() == bookId;
    });
}

// 借书：检查读者是否还有额度、资源是否可借、是否重复借，然后生成记录并更新双方状态
bool BorrowManager::borrow(Reader* reader, BookResource* resource) {
    if (!reader->canBorrow()) {
        cout << "借阅失败：已达到可借上限（" << reader->getBorrowLimit() << " 本）。\n";
        return false;
    }
    if (!resource->canBorrow()) {
        cout << "借阅失败：该资源当前无可借数量。\n";
        return false;
    }
    // 同一种资源不能重复借（避免同一资源借两本）
    if (findUnreturned(reader->getUsername(), resource->getId()) != nullptr) {
        cout << "借阅失败：你尚未归还该资源。\n";
        return false;
    }

    string today = getToday();
    string due = addDays(today, BORROW_DAYS);
    records.push_back(BorrowRecord(reader->getUsername(), resource->getId(),
                                   resource->getTitle(), resource->getType(),
                                   today, due));
    resource->borrowOne(); // 资源可借数量 -1
    reader->incBorrowed(); // 读者已借数量 +1
    save();
    cout << "借阅成功！《" << resource->getTitle() << "》 应还日期：" << due << "。\n";
    return true;
}

// 还书：找到未还记录，计算逾期罚款，恢复资源与读者状态
double BorrowManager::returnBook(Reader* reader, int bookId, BookResource* resource) {
    BorrowRecord* r = findUnreturned(reader->getUsername(), bookId);
    if (!r) {
        cout << "归还失败：没有找到你的这条未还记录。\n";
        return -1;
    }

    string today = getToday();
    r->setReturnDate(today);

    // 逾期天数 = 今天 - 应还日期（正数表示逾期）
    int overdueDays = daysBetween(r->getDueDate(), today);
    double fine = 0.0;
    if (overdueDays > 0) {
        fine = overdueDays * FINE_PER_DAY;
        cout << "该资源已逾期 " << overdueDays << " 天，需缴纳罚款 "
             << fine << " 元。\n";
    } else {
        cout << "按时归还，无需罚款。\n";
    }

    if (resource) resource->returnOne(); // 资源还在则恢复可借数量
    reader->decBorrowed();               // 读者已借数量 -1
    save();
    cout << "归还成功！《" << r->getBookTitle() << "》。\n";
    return fine;
}

// 续借：未归还且未逾期时，把应还日期再延长一个借期
bool BorrowManager::renew(Reader* reader, int bookId) {
    BorrowRecord* r = findUnreturned(reader->getUsername(), bookId);
    if (!r) {
        cout << "续借失败：没有找到你的这条未还记录。\n";
        return false;
    }
    string today = getToday();
    if (daysBetween(r->getDueDate(), today) > 0) {
        cout << "续借失败：该资源已逾期，请先归还并缴纳罚款。\n";
        return false;
    }
    r->setDueDate(addDays(r->getDueDate(), BORROW_DAYS));
    save();
    cout << "续借成功！新的应还日期：" << r->getDueDate() << "。\n";
    return true;
}

void BorrowManager::listUserRecords(const string& username) const {
    cout << "\n---------------- 我的借阅记录 ----------------\n";
    cout << left
         << setw(10) << "用户名"
         << setw(6)  << "编号"
         << setw(20) << "题名"
         << setw(10) << "类型"
         << setw(12) << "借阅日期"
         << setw(12) << "应还日期"
         << setw(12) << "归还日期\n";
    bool found = false;
    for (const auto& r : records) {
        if (r.getUsername() == username) {
            r.printInfo();
            found = true;
        }
    }
    if (!found) cout << "暂无借阅记录。\n";
    cout << "---------------------------------------------\n";
}

void BorrowManager::listAllRecords() const {
    cout << "\n---------------- 全部借阅记录 ----------------\n";
    cout << left
         << setw(10) << "用户名"
         << setw(6)  << "编号"
         << setw(20) << "题名"
         << setw(10) << "类型"
         << setw(12) << "借阅日期"
         << setw(12) << "应还日期"
         << setw(12) << "归还日期\n";
    if (records.empty()) cout << "暂无借阅记录。\n";
    for (const auto& r : records) r.printInfo();
    cout << "---------------------------------------------\n";
}

void BorrowManager::listOverdue() const {
    string today = getToday();
    cout << "\n---------------- 逾期记录 ----------------\n";
    cout << left
         << setw(10) << "用户名"
         << setw(6)  << "编号"
         << setw(20) << "题名"
         << setw(12) << "应还日期"
         << setw(8)  << "逾期天数"
         << setw(8)  << "罚款\n";
    bool found = false;
    for (const auto& r : records) {
        if (!r.isReturned()) {
            int days = daysBetween(r.getDueDate(), today);
            if (days > 0) {
                double fine = days * FINE_PER_DAY;
                cout << left
                     << setw(10) << r.getUsername()
                     << setw(6)  << r.getBookId()
                     << setw(20) << r.getBookTitle()
                     << setw(12) << r.getDueDate()
                     << setw(8)  << days
                     << setw(8)  << fine << "\n";
                found = true;
            }
        }
    }
    if (!found) cout << "当前没有逾期记录。\n";
    cout << "------------------------------------------\n";
}
