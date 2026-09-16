#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

// ============================================================
// 实验一：用户类层次结构（继承与多态）
//
// 抽象基类 User，派生类 Admin（管理员）、Reader（读者）。
// 用基类指针容器 vector<User*> 统一管理两类用户，
// 通过虚函数实现运行时多态：同一个基类指针调用，不同对象表现不同。
// ============================================================

// -------------------- 抽象基类 User --------------------
class User {
protected:
    // protected：这些成员对派生类可见，对外部不可见，体现封装
    std::string username; // 用户名（管理员账号 / 读者学号）
    std::string password; // 密码
    std::string name;     // 姓名

public:
    // 构造函数：初始化三个公共属性
    User(const std::string& u, const std::string& p, const std::string& n);

    // 虚析构函数：保证通过基类指针 delete 派生类对象时，
    // 能正确调用派生类的析构函数，避免内存泄漏。
    // 只要类要作为基类被继承，析构函数通常都应声明为 virtual。
    virtual ~User() {}

    // ---- 纯虚函数（= 0）----
    // 包含纯虚函数的类称为“抽象类”，不能直接实例化，只能被继承。
    // 派生类必须重写这些函数，否则派生类也会成为抽象类无法实例化。
    virtual std::string getRole() const = 0; // 返回角色名（"管理员"/"读者"）
    virtual void showMenu() = 0;             // 显示该角色专属菜单

    // ---- 公共接口（非虚函数，两类用户共有的行为）----
    std::string getUsername() const { return username; }
    std::string getName() const { return name; }
    bool checkPassword(const std::string& pwd) const { return pwd == password; }
    void setPassword(const std::string& pwd) { password = pwd; }
};

// -------------------- 派生类 Admin（管理员）--------------------
// 语法：class 派生类名 : public 基类名
class Admin : public User {
public:
    Admin(const std::string& u, const std::string& p, const std::string& n);

    // override 关键字：明确告诉编译器“这是重写基类的虚函数”。
    // 如果函数签名写错了，编译器会直接报错，帮助我们尽早发现低级错误。
    std::string getRole() const override;
    void showMenu() override;
};

// -------------------- 派生类 Reader（读者）--------------------
class Reader : public User {
private:
    std::string gender; // 性别
    std::string phone;  // 联系电话
    int borrowLimit;    // 可借上限
    int borrowedCount;  // 当前已借数量

public:
    Reader(const std::string& u, const std::string& p, const std::string& n,
           const std::string& g, const std::string& ph, int limit);

    std::string getRole() const override;
    void showMenu() override;

    // 读者专属方法（管理员没有这些方法，体现两类角色的差异）
    bool canBorrow() const { return borrowedCount < borrowLimit; } // 是否还能借书
    void incBorrowed() { ++borrowedCount; }                         // 已借数量 +1
    void decBorrowed() { if (borrowedCount > 0) --borrowedCount; }  // 已借数量 -1
    int getBorrowedCount() const { return borrowedCount; }
    int getBorrowLimit() const { return borrowLimit; }
    std::string getGender() const { return gender; }
    std::string getPhone() const { return phone; }
};

// -------------------- UserManager 用户管理器 --------------------
// 核心思想：用“基类指针容器”统一管理两类派生类对象，这是多态的典型应用。
class UserManager {
private:
    std::vector<User*> users; // 基类指针容器，可同时存放 Admin* 和 Reader*
public:
    UserManager();
    ~UserManager(); // 负责释放所有 new 出来的对象

    void addAdmin(const Admin& a);   // 添加管理员
    void addReader(const Reader& r); // 添加读者
    bool usernameExists(const std::string& u) const; // 用户名是否已存在
    User* login(const std::string& u, const std::string& p); // 登录校验，返回基类指针
    void listAllUsers() const; // 列出所有用户（管理员查看用）
};

#endif
