// 该指令仅对 MSVC 有效：把窄字符串字面量的编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <string>
#include <cstdlib> // system()
#include "User.h"

using namespace std;

// ==================== 控制台输入工具（实验一先用函数实现，后续实验会移入工具类）====================

// 从控制台读取一行字符串
string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// 从控制台读取一个整数（带校验，防止输入非数字导致死循环）
int readInt(const string& prompt) {
    cout << prompt;
    int n;
    while (!(cin >> n)) {        // 输入不是整数时
        cin.clear();             // 清除错误标志
        cin.ignore(10000, '\n'); // 丢弃缓冲区里的错误输入
        cout << "输入无效，请输入整数: ";
    }
    cin.ignore(10000, '\n');     // 丢弃换行符，避免影响后续的 getline
    return n;
}

// ==================== 界面函数 ====================

// 登录主菜单
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
    Reader r(u, p, name, gender, phone, 5); // 默认可借上限 5 本
    um.addReader(r);
    cout << "注册成功！\n";
}

// ==================== 主函数 ====================

int main() {
    // 把控制台代码页设为 UTF-8，保证中文正常显示（Windows 下）
    system("chcp 65001 >nul");

    UserManager um; // 创建用户管理器（构造函数里已加入默认管理员和读者）

    // 登录循环：只有选择“0. 退出系统”才会结束
    while (true) {
        int choice = showLoginMenu();

        if (choice == 1) {
            // ---------- 管理员登录 ----------
            string u = readLine("用户名: ");
            string p = readLine("密码: ");
            User* user = um.login(u, p); // 返回基类指针
            if (user != nullptr && user->getRole() == "管理员") {
                cout << "欢迎您，" << user->getName() << "（管理员）！\n";
                user->showMenu(); // 多态：基类指针调用，实际显示管理员菜单
                cout << "（该角色的具体功能将在后续实验中实现）\n";
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
                user->showMenu(); // 多态：同一句代码，这里显示读者菜单
                cout << "（该角色的具体功能将在后续实验中实现）\n";
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
