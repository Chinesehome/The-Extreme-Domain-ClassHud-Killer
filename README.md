# The Extreme Domain ClassHud Killer
A tool to manage/terminate Extreme Domain Electronic Classroom processes (studentmain.exe) for Windows.（用于管理 / 终止极域电子教室 Windows 端进程的工具 ）
![程序运行界面](https://raw.githubusercontent.com/Chinesehome/The-Extreme-Domain-ClassHud-Killer/refs/heads/main/readme/%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE_1.png)
Extreme Domain ClassHub Killer
一个用于管理极域电子教室客户端进程的工具
A tool to manage/terminate Extreme Domain Electronic Classroom processes
🌟 功能特点
智能监控：自动检测并阻止极域电子教室客户端（studentmain.exe）自启动
手动控制：通过右上角触发区一键切换进程启停状态
路径记忆：自动记录客户端路径，确保精准终止与启动
轻量高效：资源占用极低，不影响正常教学活动
🚀 使用方法
首次运行：
确保极域电子教室客户端已启动
运行本程序，它将自动记录客户端路径
控制模式：
监控模式（默认）：自动终止任何尝试启动的极域进程
运行模式：允许极域客户端正常运行
切换方法：点击屏幕右上角 30×30 像素区域
![效果](https://github.com/Chinesehome/Extreme-Domain-ClassHub-Killer/readme/屏幕截图_2.gif)
注意事项：
需以管理员权限运行以确保终止进程的有效性
首次使用需手动启动一次极域客户端以完成路径记录
💻 技术细节
开发语言：C++（Windows API）
运行环境：Windows 7/8/10/11
核心功能：
进程监控：通过 Windows 工具帮助库（ToolHelp32）实现
进程终止：调用 OpenProcess 和 TerminateProcess API
路径获取：使用 GetModuleFileNameExW 获取进程完整路径
鼠标检测：通过 GetCursorPos 和 GetAsyncKeyState 实现触发区检测
⚠️ 法律声明
本工具仅供学习和研究使用，请勿用于任何违反学校 / 机构规定或法律法规的行为。使用者需自行承担因使用本工具而产生的一切后果。作者不承担任何责任。
🤝 贡献与反馈
发现问题？请提交 Issues
有改进建议？欢迎提交 Pull Requests
📜 许可证
本项目采用 MIT License 许可协议。
📌 免责声明
本项目与极域电子教室官方无任何关联，Extreme Domain 是其官方注册商标。
