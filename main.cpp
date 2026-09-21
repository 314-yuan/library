// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <string>
#include <cstdlib>  // system()
#include <direct.h> // _mkdir()：Windows 下创建目录
#include "User.h"
#include "BookManager.h"
#include "FileUtil.h"

using namespace std;

// 数据文件路径
const string DATA_DIR = "data";
const string USERS_FILE = DATA_DIR + "/users.csv";
const string BOOKS_FILE = DATA_DIR + "/books.csv";

// 保证数据目录存在
void ensureDataDir() {
    _mkdir(DATA_DIR.c_str()); // 目录已存在时返回 -1，忽略即可
}

// 首次运行时生成默认数据文件（用户 + 图书）。
// 用临时管理器写文件，避免和后面 load() 重复加载造成数据翻倍。
void initDefaultData() {
    // 默认用户
    if (!fileExists(USERS_FILE)) {
        UserManager tmp(USERS_FILE);
        tmp.addAdmin(Admin("admin", "123456", "系统管理员"));
        tmp.addReader(Reader("2023001", "123456", "张三", "男", "13800000000", 5));
        tmp.addReader(Reader("2023002", "123456", "李四", "女", "13900000000", 5));
        tmp.save();
    }
    // 默认图书
    if (!fileExists(BOOKS_FILE)) {
        BookManager tmp(BOOKS_FILE);
        tmp.addBook(Book(1001, "C++ Primer", "Stanley B. Lippman", "人民邮电出版社", "9787115333615", "编程", 10, 10));
        tmp.addBook(Book(1002, "算法导论", "Thomas H. Cormen", "机械工业出版社", "9787111407010", "计算机", 8, 8));
        tmp.addBook(Book(1003, "三体", "刘慈欣", "重庆出版社", "9787536692930", "科幻", 15, 15));
        tmp.addBook(Book(1004, "红楼梦", "曹雪芹", "人民文学出版社", "9787020002207", "文学", 5, 5));
        tmp.addBook(Book(1005, "数据结构", "严蔚敏", "清华大学出版社", "9787302147510", "编程", 12, 12));
        tmp.save();
    }
}

// ==================== 登录主菜单 ====================
int showLoginMenu() {
    cout << "\n========== 图书馆借阅管理系统 ==========\n";
    cout << "  1. 管理员登录\n";
    cout << "  2. 读者登录\n";
    cout << "  3. 读者注册\n";
    cout << "  0. 退出系统\n";
    cout << "========================================\n";
    return readInt("请选择操作: ");
}

// 读者注册
void registerReader(UserManager& um) {
    cout << "\n---------- 读者注册 ----------\n";
    string u = readLine("用户名(学号): ");
    if (um.usernameExists(u)) {
        cout << "注册失败：该用户名已存在。\n";
        return;
    }
    string p = readLine("密码: ");
    string name = readLine("姓名: ");
    string gender = readLine("性别(男/女): ");
    string phone = readLine("联系电话: ");
    um.addReader(Reader(u, p, name, gender, phone, 5)); // 默认可借上限 5 本
    um.save();
    cout << "注册成功！\n";
}

// ==================== 管理员：图书管理子菜单 ====================
void adminBookMenu(BookManager& bm) {
    while (true) {
        cout << "\n---------- 图书管理 ----------\n";
        cout << "  1. 新增图书   2. 删除图书   3. 修改图书\n";
        cout << "  4. 查询图书   5. 列出全部   0. 返回\n";
        int c = readInt("请选择: ");
        if (c == 1) {
            bm.addBook();
        } else if (c == 2) {
            bm.deleteBook(readInt("请输入要删除的图书编号: "));
        } else if (c == 3) {
            bm.updateBook(readInt("请输入要修改的图书编号: "));
        } else if (c == 4) {
            bm.searchBooks(readLine("请输入搜索关键字: "));
        } else if (c == 5) {
            bm.listBooks();
        } else if (c == 0) {
            break;
        } else {
            cout << "无效选择，请重新输入。\n";
        }
    }
}

// ==================== 登录后的功能循环（多态：基类指针调用 showMenu）====================
void runUserLoop(User* user, UserManager& um, BookManager& bm) {
    if (dynamic_cast<Admin*>(user) != nullptr) {
        // ===== 管理员功能 =====
        while (true) {
            user->showMenu(); // 多态：显示管理员菜单
            int c = readInt("请选择: ");
            if (c == 1) {
                adminBookMenu(bm); // 图书管理（实验二实现）
            } else if (c == 2) {
                um.listAllUsers(); // 读者管理：先展示用户列表
                cout << "（读者增删改功能将在后续实验实现）\n";
            } else if (c == 3) {
                cout << "（借阅记录查询将在后续实验实现）\n";
            } else if (c == 4) {
                cout << "（逾期管理将在后续实验实现）\n";
            } else if (c == 0) {
                break;
            } else {
                cout << "无效选择，请重新输入。\n";
            }
        }
    } else if (dynamic_cast<Reader*>(user) != nullptr) {
        // ===== 读者功能 =====
        while (true) {
            user->showMenu(); // 多态：显示读者菜单
            int c = readInt("请选择: ");
            if (c == 1) {
                bm.searchBooks(readLine("请输入搜索关键字: ")); // 查询图书（实验二实现）
            } else if (c == 2 || c == 3 || c == 4 || c == 5) {
                cout << "（该功能将在后续实验实现）\n";
            } else if (c == 0) {
                break;
            } else {
                cout << "无效选择，请重新输入。\n";
            }
        }
    }
    cout << "已退出登录。\n";
}

// ==================== 主函数 ====================
int main() {
    system("chcp 65001 >nul"); // 控制台设为 UTF-8，避免中文乱码

    ensureDataDir();

    UserManager um(USERS_FILE);
    BookManager bm(BOOKS_FILE);

    initDefaultData(); // 首次运行生成默认数据文件

    um.load(); // 从 CSV 加载用户
    bm.load(); // 从 CSV 加载图书

    // 登录循环：只有选择“0. 退出系统”才会结束
    while (true) {
        int choice = showLoginMenu();

        if (choice == 1) {
            // ---------- 管理员登录 ----------
            string u = readLine("用户名: ");
            string p = readLine("密码: ");
            User* user = um.login(u, p);
            if (user != nullptr && user->getRole() == "管理员") {
                cout << "欢迎您，" << user->getName() << "（管理员）！\n";
                runUserLoop(user, um, bm);
            } else {
                cout << "登录失败：用户名或密码错误，或非管理员账号。\n";
            }
        } else if (choice == 2) {
            // ---------- 读者登录 ----------
            string u = readLine("用户名(学号): ");
            string p = readLine("密码: ");
            User* user = um.login(u, p);
            if (user != nullptr && user->getRole() == "读者") {
                cout << "欢迎您，" << user->getName() << "（读者）！\n";
                runUserLoop(user, um, bm);
            } else {
                cout << "登录失败：用户名或密码错误，或非读者账号。\n";
            }
        } else if (choice == 3) {
            registerReader(um);
        } else if (choice == 0) {
            cout << "感谢使用，再见！\n";
            break;
        } else {
            cout << "无效选择，请重新输入。\n";
        }
    }

    return 0;
}
