// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <string>
#include <cstdlib>  // system()
#include "User.h"
#include "Library.h"
#include "FileUtil.h"

using namespace std;

// 数据目录（所有 CSV 文件都放在该目录下，由 Library 统一管理）
const string DATA_DIR = "data";

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
// 参数改为 Library&：组合类作为统一入口，main 只需持有这一个对象。
void runUserLoop(User* user, Library& lib) {
    // 从 Library 取出三个成员对象的引用，供本函数使用
    UserManager& um = lib.getUsers();
    BookManager& bm = lib.getBooks();
    BorrowManager& brm = lib.getRecords();

    if (dynamic_cast<Admin*>(user) != nullptr) {
        // ===== 管理员功能 =====
        while (true) {
            user->showMenu(); // 多态：显示管理员菜单
            int c = readInt("请选择: ");
            if (c == 1) {
                adminBookMenu(bm); // 图书资源管理（支持三种资源类型）
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

    // 实验四：用 Library 组合类替代原先分散的三个管理器对象。
    // Library 内部组合了 UserManager / BookManager / BorrowManager，
    // 统一负责数据目录、默认数据生成、加载与保存。
    Library lib(DATA_DIR);
    lib.initDefaultData(); // 首次运行生成默认数据文件（内部会先确保目录存在）
    lib.load();            // 从 CSV 加载用户 / 图书资源 / 借阅记录

    UserManager& um = lib.getUsers(); // 登录与注册需要用到用户管理器

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
                runUserLoop(user, lib);
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
                runUserLoop(user, lib);
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
