#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include "LibraryModels.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void initSampleData();
    QWidget* createLoginScreen();
    QWidget* createDashboardScreen();

    void refreshBookTable(const QString& filter = "");
    void refreshBorrowTable();
    void refreshReservationTable();
    void refreshReportsTab();
    void applyRolePermissions();

private slots:
    void handleLogin();
    void handleForgotPassword();
    void handleSignUp();
    void handleLogout();
    void onReturnAndFineClicked();
    void onMarkFinePaidClicked();
    void onCancelReservationClicked();

    void onSearchTextChanged(const QString& text);
    void onBorrowBookClicked();
    void onReserveBookClicked();

private:
    QStackedWidget* m_stackedWidget;

    // Login Screen Widgets
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;

    // Dashboard Widgets
    QLabel* m_userInfoLabel;
    QTabWidget* m_dashboardTabs;

    // Tab 1: Book Catalog (Member, Librarian, Admin)
    QLineEdit* m_searchEdit;
    QTableWidget* m_bookTable;
    QPushButton* m_borrowBtn;
    QPushButton* m_reserveBtn;
    QPushButton* m_returnFineBtn;
    QPushButton* m_markPaidBtn;
    QPushButton* m_cancelReserveBtn;

    // Tab 2: Circulation & Returns (Librarian, Admin)
    QTableWidget* m_borrowTable;
    QTableWidget* m_reservationTable;

    // Tab 3: Reports & Administration (Admin)
    QLabel* m_statTotalBooks;
    QLabel* m_statActiveBorrows;
    QLabel* m_statOverdueCount;
    QLabel* m_statTotalFines;
    QTableWidget* m_usersTable;

    // In-memory Database
    User m_currentUser;
    QVector<User> m_users;
    QVector<Book> m_books;
    QVector<BorrowRecord> m_borrows;
    QVector<ReservationRecord> m_reservations;
};

#endif // MAINWINDOW_H