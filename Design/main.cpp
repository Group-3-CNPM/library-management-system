#include <QApplication>
#include "MainWindow.h"
#include <windows.h>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Áp dụng Qt Style Sheet (QSS) hiện đại cho toàn bộ ứng dụng
    app.setStyleSheet(R"(
        QWidget {
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
            color: #000000;
        }
        QMainWindow {
            background-color: #f1f5f9;
        }
        QLineEdit, QDateEdit, QTextEdit {
            border: 1px solid #cbd5e1;
            border-radius: 5px;
            padding: 6px 8px;
            background: #ffffff;
        }
        QLineEdit:focus, QDateEdit:focus, QTextEdit:focus {
            border: 1px solid #2563eb;
        }
        QPushButton {
            background-color: #e2e8f0;
            border: none;
            border-radius: 5px;
            padding: 7px 14px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #cbd5e1;
        }
        QPushButton#primaryButton {
            background-color: #2563eb;
            color: white;
        }
        QPushButton#primaryButton:hover {
            background-color: #1d4ed8;
        }
        QPushButton#successButton {
            background-color: #16a34a;
            color: white;
        }
        QPushButton#successButton:hover {
            background-color: #15803d;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #cbd5e1;
            border-radius: 5px;
            gridline-color: #e2e8f0;
        }
        QHeaderView::section {
            background-color: #f8fafc;
            padding: 6px;
            border: none;
            border-bottom: 1px solid #cbd5e1;
            font-weight: bold;
        }
    )");

    MainWindow window;
    window.show();

    return app.exec();
}
//set "PATH=C:\msys64\ucrt64\bin;%PATH%" && C:\LibraryManagementApp\Code\Design\build\LibraryManagementApp.exe
// cmake --build build