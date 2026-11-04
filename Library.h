#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include "User.h"
#include "BookManager.h"
#include "BorrowManager.h"

// ============================================================
// 实验四：Library 图书馆组合类（组合关系 + 多态与虚函数）
//
// 组合（Composition）：Library 由三个成员对象组成，它们属于 Library，
// 与 Library 同生同灭，是典型的“整体 - 部分”关系——
//   UserManager    users   用户管理（内部管理 vector<User*>）
//   BookManager    books   图书资源管理（内部管理 vector<BookResource*>）
//   BorrowManager  records 借阅管理（内部管理 vector<BorrowRecord>）
//
// Library 作为整个系统的统一入口（外观模式 Facade）：main 只需要操作
// 一个 Library 对象，不必关心内部有多少个管理器、数据文件放在哪里。
//
// 多态体现：BookManager 用基类指针 vector<BookResource*> 统一管理
// 一般图书 / 杂志 / 电子资源，通过虚函数 showInfo() / toCSV() / getType()
// 和虚析构函数实现“同一句代码、不同对象、不同行为”的运行时多态。
// ============================================================
class Library {
private:
    std::string dataDir;     // 数据目录（如 "data"）
    UserManager users;       // 组合成员：用户管理
    BookManager books;       // 组合成员：图书资源管理
    BorrowManager records;   // 组合成员：借阅管理

public:
    // 构造：传入数据目录，内部拼出三个 CSV 文件路径并初始化三个成员
    Library(const std::string& dir);

    // 确保数据目录存在（Windows 下用 _mkdir，已存在则忽略）
    void ensureDataDir();

    // 首次运行生成默认数据（用户 + 图书资源）；文件已存在则跳过
    void initDefaultData();

    // 加载 / 保存全部数据（内部依次调用三个成员管理器的 load/save）
    void load();
    void save() const;

    // 访问器：返回三个成员对象的引用，供 main 调用各自的具体功能
    UserManager& getUsers()     { return users; }
    BookManager& getBooks()     { return books; }
    BorrowManager& getRecords() { return records; }
};

#endif
