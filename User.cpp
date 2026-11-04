// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "User.h"
#include "FileUtil.h"
#include "TemplateUtil.h" // 实验五：通用模板函数 findPtr / forEach
#include <iostream>

using namespace std;

// ==================== 基类 User ====================

User::User(const string& u, const string& p, const string& n)
    : username(u), password(p), name(n) {}

// ==================== 派生类 Admin ====================

Admin::Admin(const string& u, const string& p, const string& n)
    : User(u, p, n) {}

string Admin::getRole() const { return "管理员"; }

void Admin::showMenu() {
    cout << "\n================ 管理员菜单 ================\n";
    cout << "  1. 图书管理\n";
    cout << "  2. 读者管理\n";
    cout << "  3. 借阅记录查询\n";
    cout << "  4. 逾期管理\n";
    cout << "  0. 退出登录\n";
    cout << "===========================================\n";
}

// 管理员序列化为 CSV（统一 8 列，管理员不用的字段填空或 0）
vector<string> Admin::toCSV() const {
    return { "admin", username, password, name, "", "", "0", "0" };
}

// ==================== 派生类 Reader ====================

Reader::Reader(const string& u, const string& p, const string& n,
               const string& g, const string& ph, int limit)
    : User(u, p, n), gender(g), phone(ph),
      borrowLimit(limit), borrowedCount(0) {}

string Reader::getRole() const { return "读者"; }

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

vector<string> Reader::toCSV() const {
    return { "reader", username, password, name, gender, phone,
             to_string(borrowLimit), to_string(borrowedCount) };
}

// ==================== UserManager ====================

UserManager::UserManager(const string& filePath) : path(filePath) {}

UserManager::~UserManager() {
    // 遍历基类指针容器逐个释放；虚析构保证派生类被正确销毁
    for (auto u : users) {
        delete u;
    }
}

// 从 CSV 加载所有用户
void UserManager::load() {
    for (const auto& row : readCSV(path)) {
        User* u = createUser(row); // 工厂式创建
        if (u) users.push_back(u);
    }
}

// 把所有用户写回 CSV（多态：不同角色生成不同行）
void UserManager::save() const {
    vector<vector<string>> rows;
    for (const auto& u : users) {
        rows.push_back(u->toCSV());
    }
    writeCSV(path, rows);
}

// 工厂式创建：根据第一列“角色”决定 new 出 Admin 还是 Reader，返回基类指针
User* UserManager::createUser(const vector<string>& f) {
    // f 各列：角色,用户名,密码,姓名,性别,电话,借阅上限,已借数量
    if (f.size() < 4) return nullptr;
    if (f[0] == "admin") {
        return new Admin(f[1], f[2], f[3]);
    } else if (f[0] == "reader") {
        int limit = f.size() > 6 ? toInt(f[6], 5) : 5;
        int count = f.size() > 7 ? toInt(f[7], 0) : 0;
        Reader* r = new Reader(f[1], f[2], f[3],
                               f.size() > 4 ? f[4] : "",
                               f.size() > 5 ? f[5] : "", limit);
        r->setBorrowedCount(count); // 恢复已借数量
        return r;
    }
    return nullptr;
}

void UserManager::addAdmin(const Admin& a) {
    users.push_back(new Admin(a));
}

void UserManager::addReader(const Reader& r) {
    users.push_back(new Reader(r));
}

bool UserManager::usernameExists(const string& u) const {
    // findPtr 查找第一个用户名匹配的元素，找到则返回非空指针
    return findPtr(users, [&u](User* x) { return x->getUsername() == u; }) != nullptr;
}

User* UserManager::login(const string& u, const string& p) {
    // findPtr 查找用户名与密码都匹配的用户
    return findPtr(users, [&](User* x) {
        return x->getUsername() == u && x->checkPassword(p);
    });
}

void UserManager::listAllUsers() const {
    cout << "\n---------------- 用户列表 ----------------\n";
    // forEach 模板遍历；u->getRole() 是虚函数调用，多态返回不同角色名
    forEach(users, [](User* u) {
        cout << "[" << u->getRole() << "] " << u->getUsername()
             << "  " << u->getName() << "\n";
    });
    cout << "------------------------------------------\n";
}
