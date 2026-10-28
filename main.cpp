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
#include "BorrowManager.h"
#include "FileUtil.h"

using namespace std;

// 数据文件路径
const string DATA_DIR = "data";
const string USERS_FILE = DATA_DIR + "/users.csv";
const string BOOKS_FILE = DATA_DIR + "/books.csv";
const string RECORDS_FILE = DATA_DIR + "/records.csv";

// 保证数据目录存在
void ensureDataDir() {
    _mkdir(DATA_DIR.c_str()); // 目录已存在时返回 -1，忽略即可
}

// 首次运行时生成默认数据文件（用户 + 图书资源）。
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
    // 默认图书资源：一般图书 + 杂志 + 电子资源（体现继承派生）
    if (!fileExists(BOOKS_FILE)) {
        BookManager tmp(BOOKS_FILE);
        // 一般图书
        tmp.addBook(new GeneralBook(1001, "C++ Primer", "人民邮电出版社", 10, 10,
                                    "Stanley B. Lippman", "9787115333615", "编程"));
        tmp.addBook(new GeneralBook(1002, "算法导论", "机械工业出版社", 8, 8,
                                    "Thomas H. Cormen", "9787111407010", "计算机"));
        tmp.addBook(new GeneralBook(1003, "三体", "重庆出版社", 15, 15,
                                    "刘慈欣", "9787536692930", "科幻"));
        tmp.addBook(new GeneralBook(1004, "红楼梦", "人民文学出版社", 5, 5,
                                    "曹雪芹", "9787020002207", "文学"));
        // 杂志
        tmp.addBook(new Magazine(2001, "程序员", "CSDN", 20, 20, "2026-10", "1672-5945"));
        tmp.addBook(new Magazine(2002, "读者", "读者出版集团", 30, 30, "2026-09", "1005-1803"));
        // 电子资源
        tmp.addBook(new EResource(3001, "C++标准库(电子版)", "电子工业出版社", 50, 50,
                                  "PDF", "http://lib.example.com/3001"));
        tmp.addBook(new EResource(3002, "深度学习综述", "中国知网", 99, 99,
                                  "CAJ", "http://cnki.net/3002"));
        tmp.save();
    }
    // 借阅记录文件首次运行时为空，无需预生成（BorrowManager::load 读不到文件返回空）。
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
        cout << "  1. 新增资源   2. 删除资源   3. 修改资源\n";
        cout << "  4. 查询资源   5. 列出全部   0. 返回\n";
        int c = readInt("请选择: ");
        if (c == 1) {
            bm.addBook();
        } else if (c == 2) {
            bm.deleteBook(readInt("请输入要删除的资源编号: "));
        } else if (c == 3) {
            bm.updateBook(readInt("请输入要修改的资源编号: "));
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
void runUserLoop(User* user, UserManager& um, BookManager& bm, BorrowManager& brm) {
    if (dynamic_cast<Admin*>(user) != nullptr) {
        // ===== 管理员功能 =====
        while (true) {
            user->showMenu(); // 多态：显示管理员菜单
            int c = readInt("请选择: ");
            if (c == 1) {
                adminBookMenu(bm); // 图书资源管理（实验三：支持三种资源类型）
            } else if (c == 2) {
                um.listAllUsers(); // 读者管理：先展示用户列表
                cout << "（读者增删改功能将在后续实验实现）\n";
            } else if (c == 3) {
                brm.listAllRecords(); // 借阅记录查询
            } else if (c == 4) {
                brm.listOverdue();    // 逾期管理：列出逾期记录与罚款
            } else if (c == 0) {
                break;
            } else {
                cout << "无效选择，请重新输入。\n";
            }
        }
    } else if (dynamic_cast<Reader*>(user) != nullptr) {
        // ===== 读者功能 =====
        Reader* reader = dynamic_cast<Reader*>(user);
        while (true) {
            user->showMenu(); // 多态：显示读者菜单
            int c = readInt("请选择: ");
            if (c == 1) {
                bm.searchBooks(readLine("请输入搜索关键字: ")); // 查询图书
            } else if (c == 2) {
                // 借书：先列出全部资源，方便查看编号
                bm.listBooks();
                int id = readInt("请输入要借的资源编号: ");
                BookResource* res = bm.findBook(id);
                if (!res) {
                    cout << "未找到编号为 " << id << " 的资源。\n";
                } else if (brm.borrow(reader, res)) {
                    bm.save(); // 资源可借数量变化写回 books.csv
                    um.save(); // 读者已借数量变化写回 users.csv
                }
            } else if (c == 3) {
                // 还书
                int id = readInt("请输入要归还的资源编号: ");
                BookResource* res = bm.findBook(id);
                double fine = brm.returnBook(reader, id, res);
                if (fine >= 0) {
                    bm.save(); // 资源可借数量恢复写回
                    um.save(); // 读者已借数量恢复写回
                }
            } else if (c == 4) {
                // 续借
                brm.renew(reader, readInt("请输入要续借的资源编号: "));
            } else if (c == 5) {
                brm.listUserRecords(reader->getUsername()); // 我的借阅记录
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
    BorrowManager brm(RECORDS_FILE);

    initDefaultData(); // 首次运行生成默认数据文件

    um.load();  // 从 CSV 加载用户
    bm.load();  // 从 CSV 加载图书资源
    brm.load(); // 从 CSV 加载借阅记录

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
                runUserLoop(user, um, bm, brm);
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
                runUserLoop(user, um, bm, brm);
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
