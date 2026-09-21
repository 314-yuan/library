#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

// ============================================================
// 用户类层次结构（继承与多态）
//
// 抽象基类 User，派生类 Admin（管理员）、Reader（读者）。
// 用基类指针容器 vector<User*> 统一管理两类用户，
// 通过虚函数实现运行时多态：同一个基类指针调用，不同对象表现不同。
// ============================================================

// -------------------- 抽象基类 User --------------------
class User {
protected:
    std::string username; // 用户名（管理员账号 / 读者学号）
    std::string password; // 密码
    std::string name;     // 姓名

public:
    User(const std::string& u, const std::string& p, const std::string& n);

    // 虚析构函数：保证通过基类指针 delete 派生类对象时能正确析构
    virtual ~User() {}

    // ---- 纯虚函数（= 0）----
    // 包含纯虚函数的类称为“抽象类”，不能直接实例化，只能被继承。
    virtual std::string getRole() const = 0;             // 返回角色名
    virtual void showMenu() = 0;                         // 显示该角色专属菜单
    virtual std::vector<std::string> toCSV() const = 0;  // 序列化为 CSV 一行

    // ---- 公共接口（非虚函数）----
    std::string getUsername() const { return username; }
    std::string getName() const { return name; }
    bool checkPassword(const std::string& pwd) const { return pwd == password; }
    void setPassword(const std::string& pwd) { password = pwd; }
};

// -------------------- 派生类 Admin（管理员）--------------------
class Admin : public User {
public:
    Admin(const std::string& u, const std::string& p, const std::string& n);

    std::string getRole() const override;
    void showMenu() override;
    std::vector<std::string> toCSV() const override;
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
    std::vector<std::string> toCSV() const override;

    // 读者专属方法（管理员没有，体现两类角色的差异）
    bool canBorrow() const { return borrowedCount < borrowLimit; } // 是否还能借书
    void incBorrowed() { ++borrowedCount; }                         // 已借数量 +1
    void decBorrowed() { if (borrowedCount > 0) --borrowedCount; }  // 已借数量 -1
    int getBorrowedCount() const { return borrowedCount; }
    int getBorrowLimit() const { return borrowLimit; }
    std::string getGender() const { return gender; }
    std::string getPhone() const { return phone; }
    void setBorrowedCount(int c) { borrowedCount = c; } // 从文件加载时恢复
};

// -------------------- UserManager 用户管理器 --------------------
class UserManager {
private:
    std::vector<User*> users; // 基类指针容器，可同时存放 Admin* 和 Reader*
    std::string path;         // 数据文件路径（data/users.csv）

public:
    UserManager(const std::string& filePath);
    ~UserManager(); // 负责释放所有 new 出来的对象

    // ---------- 文件读写（持久化）----------
    void load();       // 从 CSV 加载所有用户
    void save() const; // 把所有用户写回 CSV

    // ---------- 用户管理 ----------
    void addAdmin(const Admin& a);   // 添加管理员
    void addReader(const Reader& r); // 添加读者
    bool usernameExists(const std::string& u) const; // 用户名是否已存在
    User* login(const std::string& u, const std::string& p); // 登录校验
    void listAllUsers() const; // 列出所有用户（管理员查看用）

    // 工厂式创建：根据“角色”字段 new 出正确的派生类（体现多态）
    static User* createUser(const std::vector<std::string>& fields);
};

#endif
