// 该指令仅对 MSVC 有效：把窄字符串字面量的编码指定为 UTF-8，
// 保证中文在控制台（配合 chcp 65001）中正确显示。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "User.h"
#include <iostream>

using namespace std;

// ==================== 基类 User ====================

// 构造函数使用“初始化列表”初始化成员变量，效率更高、更规范
User::User(const string& u, const string& p, const string& n)
    : username(u), password(p), name(n) {}

// ==================== 派生类 Admin ====================

// 派生类构造函数：先调用基类构造函数初始化基类部分，再初始化自己的成员
Admin::Admin(const string& u, const string& p, const string& n)
    : User(u, p, n) {}

// 重写基类纯虚函数：返回角色名
string Admin::getRole() const {
    return "管理员";
}

// 重写基类纯虚函数：显示管理员菜单
void Admin::showMenu() {
    cout << "\n================ 管理员菜单 ================\n";
    cout << "  1. 图书管理\n";
    cout << "  2. 读者管理\n";
    cout << "  3. 借阅记录查询\n";
    cout << "  4. 逾期管理\n";
    cout << "  0. 退出登录\n";
    cout << "===========================================\n";
}

// ==================== 派生类 Reader ====================

Reader::Reader(const string& u, const string& p, const string& n,
               const string& g, const string& ph, int limit)
    : User(u, p, n),          // 先初始化基类部分
      gender(g), phone(ph),   // 再初始化派生类自己的成员
      borrowLimit(limit), borrowedCount(0) {}

string Reader::getRole() const {
    return "读者";
}

void Reader::showMenu() {
    cout << "\n================ 读者菜单 ================\n";
    cout << "  1. 查询图书\n";
    cout << "  2. 借书\n";
    cout << "  3. 还书\n";
    cout << "  4. 续借\n";
    cout << "  5. 我的借阅记录\n";
    cout << "  0. 退出登录\n";
    cout << "=========================================\n";
}

// ==================== UserManager ====================

UserManager::UserManager() {
    // 实验一：先用内存中的默认数据演示（实验二改为从 CSV 文件加载）
    addAdmin(Admin("admin", "123456", "系统管理员"));
    addReader(Reader("2023001", "123456", "张三", "男", "13800000000", 5));
    addReader(Reader("2023002", "123456", "李四", "女", "13900000000", 5));
}

UserManager::~UserManager() {
    // 遍历基类指针容器，逐个释放。
    // 因为 User 的析构函数是 virtual 的，这里 delete 基类指针时
    // 会正确调用对应派生类的析构函数，不会内存泄漏。
    for (auto u : users) {
        delete u;
    }
}

void UserManager::addAdmin(const Admin& a) {
    // new 一个 Admin，把它的基类指针 push 进容器（向上转型，隐式发生）
    users.push_back(new Admin(a));
}

void UserManager::addReader(const Reader& r) {
    users.push_back(new Reader(r));
}

bool UserManager::usernameExists(const string& u) const {
    for (auto x : users) {
        if (x->getUsername() == u) return true;
    }
    return false;
}

User* UserManager::login(const string& u, const string& p) {
    for (auto x : users) {
        // 用户名匹配且密码正确
        if (x->getUsername() == u && x->checkPassword(p)) {
            return x; // 返回基类指针（可能是 Admin* 或 Reader*）
        }
    }
    return nullptr; // 登录失败
}

void UserManager::listAllUsers() const {
    cout << "\n---------------- 用户列表 ----------------\n";
    for (const auto& u : users) {
        // u->getRole() 是虚函数调用：
        // 同一句代码，u 实际指向 Admin 时返回“管理员”，指向 Reader 时返回“读者”
        cout << "[" << u->getRole() << "] " << u->getUsername()
             << "  " << u->getName() << "\n";
    }
    cout << "------------------------------------------\n";
}
