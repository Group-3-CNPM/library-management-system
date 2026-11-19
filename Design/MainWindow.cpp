#include "MainWindow.h"
#include "Dialogs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QLocale>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Library Management System - Qt Widgets");
    resize(980, 640);

    initSampleData();

    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->addWidget(createLoginScreen());     // Index 0: Login Screen
    m_stackedWidget->addWidget(createDashboardScreen()); // Index 1: Dashboard Screen

    setCentralWidget(m_stackedWidget);
    m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::initSampleData() {
    // 3 tài khoản đại diện cho 3 phân quyền
    m_users = {
        {"member",    "123", UserRole::Member,        "MEM-101"},
        {"librarian", "123", UserRole::Librarian,     "LIB-001"},
        {"admin",     "123", UserRole::Administrator, "ADM-001"}
    };

    // Danh mục sách mẫu (có sách hết hàng để test tính năng Reserve Book)
    m_books = {
        {"978-0132350884", "Clean Code", "Robert C. Martin", 3, 0}, // Hết sách -> Dùng Reserve
        {"978-0201633610", "Design Patterns", "Erich Gamma et al.", 5, 4},
        {"978-0131103627", "The C Programming Language", "Brian W. Kernighan", 4, 2},
        {"978-1491950357", "Designing Data-Intensive Applications", "Martin Kleppmann", 2, 1}
    };

    // Giao dịch mượn mẫu (có 1 phiếu quá hạn 4 ngày để test Return & Fine)
    m_borrows = {
        {"BRW-001", "MEM-101", "978-0132350884", "Clean Code",
         QDate::currentDate().addDays(-18), QDate::currentDate().addDays(-4), false, 0.0},
        {"BRW-002", "MEM-102", "978-0201633610", "Design Patterns",
         QDate::currentDate().addDays(-3), QDate::currentDate().addDays(11), false, 0.0}
    };

    m_reservations = {
        {"RES-001", "MEM-105", "978-0132350884", "Clean Code",
         QDate::currentDate().addDays(-1), "Need for final capstone project"}
    };
}

// ============================================================================
// 1. LOGIN SCREEN
// ============================================================================
QWidget* MainWindow::createLoginScreen() {
    auto* container = new QWidget(this);
    auto* outerLayout = new QVBoxLayout(container);
    outerLayout->setAlignment(Qt::AlignCenter);

    auto* loginCard = new QGroupBox(container);
    loginCard->setFixedWidth(380);
    loginCard->setStyleSheet("QGroupBox { background: white; border-radius: 8px; padding: 24px; border: 1px solid #dcdcdc; }");

    auto* cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setSpacing(14);

    auto* titleLbl = new QLabel("LIBRARY PORTAL LOGIN", loginCard);
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size: 20px; font-weight: bold; color: #000000; margin-bottom: 6px;");

    auto* subTitleLbl = new QLabel("Sign in with Member, Librarian, or Admin account", loginCard);
    subTitleLbl->setAlignment(Qt::AlignCenter);
    subTitleLbl->setStyleSheet("font-size: 12px; color: #64748b; margin-bottom: 10px;");

    m_usernameEdit = new QLineEdit(loginCard);
    m_usernameEdit->setPlaceholderText("Username");
    m_usernameEdit->setMinimumHeight(36);

    m_passwordEdit = new QLineEdit(loginCard);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(36);

    auto* loginBtn = new QPushButton("Log In", loginCard);
    loginBtn->setObjectName("primaryButton");
    loginBtn->setMinimumHeight(38);
    loginBtn->setCursor(Qt::PointingHandCursor);

    // Liên kết "Forgot password?" và "Sign up"
    auto* linksLayout = new QHBoxLayout();
    auto* forgotBtn = new QPushButton("Forgot password?", loginCard);
    auto* signUpBtn = new QPushButton("Sign up", loginCard);

    QString linkStyle = "QPushButton { border: none; color: #2563eb; background: transparent; text-decoration: underline; font-size: 12px; }"
                        "QPushButton:hover { color: #1d4ed8; }";
    forgotBtn->setStyleSheet(linkStyle);
    signUpBtn->setStyleSheet(linkStyle);
    forgotBtn->setCursor(Qt::PointingHandCursor);
    signUpBtn->setCursor(Qt::PointingHandCursor);

    linksLayout->addWidget(forgotBtn);
    linksLayout->addStretch();
    linksLayout->addWidget(signUpBtn);

    cardLayout->addWidget(titleLbl);
    cardLayout->addWidget(subTitleLbl);
    cardLayout->addWidget(new QLabel("Username:", loginCard));
    cardLayout->addWidget(m_usernameEdit);
    cardLayout->addWidget(new QLabel("Password:", loginCard));
    cardLayout->addWidget(m_passwordEdit);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(loginBtn);
    cardLayout->addLayout(linksLayout);

    outerLayout->addWidget(loginCard);

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::handleLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);
    connect(forgotBtn, &QPushButton::clicked, this, &MainWindow::handleForgotPassword);
    connect(signUpBtn, &QPushButton::clicked, this, &MainWindow::handleSignUp);

    return container;
}

void MainWindow::onCancelReservationClicked() {
    int row = m_reservationTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Reservation", "Please select a reservation request to cancel.");
        return;
    }

    QString resId = m_reservationTable->item(row, 0)->text();
    for (int i = 0; i < m_reservations.size(); ++i) {
        if (m_reservations[i].reserveId == resId) {
            if (m_currentUser.role == UserRole::Member &&
                m_reservations[i].memberId != m_currentUser.memberId) {
                QMessageBox::warning(this, "Access Denied", "You can only cancel your own reservations.");
                return;
            }
            m_reservations.removeAt(i);
            refreshReservationTable();
            QMessageBox::information(this, "Cancelled",
                                     QString("Reservation %1 has been cancelled.").arg(resId));
            return;
        }
    }
}

void MainWindow::handleLogin() {
    QString uname = m_usernameEdit->text().trimmed();
    QString pwd = m_passwordEdit->text();

    for (const auto& user : m_users) {
        if (user.username == uname && user.password == pwd) {
            m_currentUser = user;
            m_userInfoLabel->setText(QString("Logged in as: <b>%1</b> | Role: <span style='color:#2563eb; font-weight:bold;'>%2</span>")
                                         .arg(m_currentUser.username, roleToString(m_currentUser.role)));
            applyRolePermissions();
            refreshBookTable();
            refreshBorrowTable();
            refreshReservationTable();
            refreshReportsTab();

            m_stackedWidget->setCurrentIndex(1);
            return;
        }
    }
    QMessageBox::warning(this, "Login Failed", "Invalid Username or Password.\nTry: member/123, librarian/123, or admin/123.");
}

void MainWindow::handleForgotPassword() {
    bool ok;
    QString uname = QInputDialog::getText(this, "Forgot Password",
                                          "Enter your registered Username to reset password:",
                                          QLineEdit::Normal, "", &ok);
    if (ok && !uname.trimmed().isEmpty()) {
        QMessageBox::information(this, "Password Reset",
                                 QString("A password reset link has been sent to the email associated with '%1'.").arg(uname.trimmed()));
    }
}

void MainWindow::handleSignUp() {
    bool ok;
    QString newUser = QInputDialog::getText(this, "Member Sign Up", "Choose a new Username:", QLineEdit::Normal, "", &ok);
    if (!ok || newUser.trimmed().isEmpty()) return;

    QString newPass = QInputDialog::getText(this, "Member Sign Up", "Choose a Password:", QLineEdit::Password, "", &ok);
    if (!ok || newPass.isEmpty()) return;

    QString newMemId = QString("MEM-%1").arg(100 + m_users.size() + 1);
    m_users.append({newUser.trimmed(), newPass, UserRole::Member, newMemId});
    QMessageBox::information(this, "Registration Successful",
                             QString("Account created! Your Member ID is %1. You can now log in.").arg(newMemId));
}

void MainWindow::handleLogout() {
    m_usernameEdit->clear();
    m_passwordEdit->clear();
    m_stackedWidget->setCurrentIndex(0);
}

// ============================================================================
// 2. DASHBOARD / MAIN WINDOW (PHÂN QUYỀN MEMBER / LIBRARIAN / ADMIN)
// ============================================================================
QWidget* MainWindow::createDashboardScreen() {
    auto* dashboard = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(dashboard);

    // Top Header Bar
    auto* headerLayout = new QHBoxLayout();
    auto* brandLabel = new QLabel("<h2>Library Management Dashboard</h2>", dashboard);
    m_userInfoLabel = new QLabel(dashboard);
    auto* logoutBtn = new QPushButton("Log Out", dashboard);

    headerLayout->addWidget(brandLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_userInfoLabel);
    headerLayout->addSpacing(12);
    headerLayout->addWidget(logoutBtn);

    m_dashboardTabs = new QTabWidget(dashboard);

    // ------------------------------------------------------------------------
    // TAB 1: BOOK CATALOG (Tra cứu, Mượn & Đặt trước sách)
    // ------------------------------------------------------------------------
    auto* catalogTab = new QWidget();
    auto* catalogLayout = new QVBoxLayout(catalogTab);

    auto* searchBarLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search by Book Title, Author, or ISBN...");
    m_borrowBtn = new QPushButton("Borrow Book");
    m_borrowBtn->setObjectName("primaryButton");
    m_reserveBtn = new QPushButton("Reserve Book");

    searchBarLayout->addWidget(m_searchEdit);
    searchBarLayout->addWidget(m_borrowBtn);
    searchBarLayout->addWidget(m_reserveBtn);

    m_bookTable = new QTableWidget(0, 5);
    m_bookTable->setHorizontalHeaderLabels({"ISBN", "Title", "Author", "Available", "Status"});
    m_bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_bookTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    catalogLayout->addLayout(searchBarLayout);
    catalogLayout->addWidget(m_bookTable);

    // ------------------------------------------------------------------------
    // TAB 2: CIRCULATION & RETURNS (Duyệt Mượn/Trả & Tính phạt - Librarian/Admin)
    // ------------------------------------------------------------------------
    auto* circulationTab = new QWidget();
    auto* circLayout = new QVBoxLayout(circulationTab);

    auto* circActionLayout = new QHBoxLayout();
    circActionLayout->addWidget(new QLabel("<b>Active Borrow Transactions</b>"));
    circActionLayout->addStretch();
    m_markPaidBtn = new QPushButton("Mark Fine Paid");
    m_returnFineBtn = new QPushButton("Return Book & Calculate Fine");
    m_returnFineBtn->setObjectName("successButton");
    circActionLayout->addWidget(m_markPaidBtn);
    circActionLayout->addWidget(m_returnFineBtn);

    m_borrowTable = new QTableWidget(0, 8);
    m_borrowTable->setHorizontalHeaderLabels({
        "Record ID", "Member ID", "ISBN", "Book Title",
        "Borrow Date", "Due Date", "Status", "Fine Status"
    });
    m_borrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_borrowTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_borrowTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_borrowTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* resHeaderLayout = new QHBoxLayout();
    resHeaderLayout->addWidget(new QLabel("<b>Pending Book Reservations</b>"));
    resHeaderLayout->addStretch();
    m_cancelReserveBtn = new QPushButton("Cancel Reservation");
    resHeaderLayout->addWidget(m_cancelReserveBtn);

    m_reservationTable = new QTableWidget(0, 6);
    m_reservationTable->setHorizontalHeaderLabels({"Reserve ID", "Member ID", "ISBN", "Book Title", "Reserve Date", "Notes"});
    m_reservationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reservationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reservationTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_reservationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    circLayout->addLayout(circActionLayout);
    circLayout->addWidget(m_borrowTable);
    circLayout->addLayout(resHeaderLayout);
    circLayout->addWidget(m_reservationTable);

    // ------------------------------------------------------------------------
    // TAB 3: REPORTS & ADMINISTRATION (Báo cáo & Quản trị - Administrator)
    // ------------------------------------------------------------------------
    auto* adminTab = new QWidget();
    auto* adminLayout = new QVBoxLayout(adminTab);

    auto* statsGroup = new QGroupBox("System Summary Report");
    auto* statsLayout = new QHBoxLayout(statsGroup);

    m_statTotalBooks = new QLabel();
    m_statActiveBorrows = new QLabel();
    m_statOverdueCount = new QLabel();
    m_statTotalFines = new QLabel();

    statsLayout->addWidget(m_statTotalBooks);
    statsLayout->addWidget(m_statActiveBorrows);
    statsLayout->addWidget(m_statOverdueCount);
    statsLayout->addWidget(m_statTotalFines);

    m_usersTable = new QTableWidget(0, 3);
    m_usersTable->setHorizontalHeaderLabels({"Username", "Role", "Member / Staff ID"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    adminLayout->addWidget(statsGroup);
    adminLayout->addWidget(new QLabel("<b>System Accounts & Role Assignments</b>"));
    adminLayout->addWidget(m_usersTable);

    m_dashboardTabs->addTab(catalogTab, "1. Book Catalog & Search");
    m_dashboardTabs->addTab(circulationTab, "2. Borrow / Return & Fines");
    m_dashboardTabs->addTab(adminTab, "3. Reports & Administration");

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(m_dashboardTabs);

    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::handleLogout);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(m_borrowBtn, &QPushButton::clicked, this, &MainWindow::onBorrowBookClicked);
    connect(m_reserveBtn, &QPushButton::clicked, this, &MainWindow::onReserveBookClicked);
    connect(m_returnFineBtn, &QPushButton::clicked, this, &MainWindow::onReturnAndFineClicked);
    connect(m_returnFineBtn, &QPushButton::clicked, this, &MainWindow::onReturnAndFineClicked);
    connect(m_markPaidBtn, &QPushButton::clicked, this, &MainWindow::onMarkFinePaidClicked);
    connect(m_cancelReserveBtn, &QPushButton::clicked, this, &MainWindow::onCancelReservationClicked);
    return dashboard;
}

void MainWindow::applyRolePermissions() {
    bool isMember = (m_currentUser.role == UserRole::Member);
    bool isAdmin  = (m_currentUser.role == UserRole::Administrator);

    // Tab 1: Tất cả vai trò
    m_dashboardTabs->setTabEnabled(0, true);
    // Tab 2: Member xem lịch sử mượn, yêu cầu trả sách, hủy đặt trước; Librarian/Admin duyệt trả & thu phạt
    m_dashboardTabs->setTabEnabled(1, true);
    // Tab 3: Librarian & Admin xem báo cáo (FR10, UC-15); chỉ Admin quản lý tài khoản (UC-18)
    m_dashboardTabs->setTabEnabled(2, !isMember);
    // Phân quyền nút thao tác bên trong Tab 2
    m_markPaidBtn->setVisible(!isMember);
    m_returnFineBtn->setText(isMember ? "Request Return / View Fine" : "Return Book & Calculate Fine");

    // Chỉ Administrator mới thấy bảng quản trị tài khoản ở Tab 3
    m_usersTable->setVisible(isAdmin);
    m_dashboardTabs->setCurrentIndex(0);
}

void MainWindow::refreshBookTable(const QString& filter) {
    m_bookTable->setRowCount(0);
    for (const auto& book : m_books) {
        if (!filter.isEmpty() &&
            !book.title.contains(filter, Qt::CaseInsensitive) &&
            !book.author.contains(filter, Qt::CaseInsensitive) &&
            !book.isbn.contains(filter, Qt::CaseInsensitive)) {
            continue;
        }
        int row = m_bookTable->rowCount();
        m_bookTable->insertRow(row);
        m_bookTable->setItem(row, 0, new QTableWidgetItem(book.isbn));
        m_bookTable->setItem(row, 1, new QTableWidgetItem(book.title));
        m_bookTable->setItem(row, 2, new QTableWidgetItem(book.author));
        m_bookTable->setItem(row, 3, new QTableWidgetItem(QString("%1 / %2").arg(book.availableCopies).arg(book.totalCopies)));
        m_bookTable->setItem(row, 4, new QTableWidgetItem(book.availableCopies > 0 ? "Available" : "Unavailable"));
    }
}

void MainWindow::refreshBorrowTable() {
    QLocale vn(QLocale::Vietnamese);
    m_borrowTable->setRowCount(0);
    bool isMember = (m_currentUser.role == UserRole::Member);

    for (const auto& rec : m_borrows) {
        if (isMember && rec.memberId != m_currentUser.memberId) {
            continue;
        }
        int row = m_borrowTable->rowCount();
        m_borrowTable->insertRow(row);
        m_borrowTable->setItem(row, 0, new QTableWidgetItem(rec.recordId));
        m_borrowTable->setItem(row, 1, new QTableWidgetItem(rec.memberId));
        m_borrowTable->setItem(row, 2, new QTableWidgetItem(rec.isbn));
        m_borrowTable->setItem(row, 3, new QTableWidgetItem(rec.bookTitle));
        m_borrowTable->setItem(row, 4, new QTableWidgetItem(rec.borrowDate.toString("dd/MM/yyyy")));
        m_borrowTable->setItem(row, 5, new QTableWidgetItem(rec.dueDate.toString("dd/MM/yyyy")));

        QString status = rec.isReturned ? "Returned"
                         : (rec.dueDate < QDate::currentDate() ? "Overdue" : "Borrowed");
        m_borrowTable->setItem(row, 6, new QTableWidgetItem(status));

        double accruedFine = rec.fineAmount;
        if (!rec.isReturned && rec.dueDate < QDate::currentDate()) {
            int overdueDays = rec.dueDate.daysTo(QDate::currentDate());
            accruedFine = overdueDays * 5000.0;
        }

        QString fineStatusText = "No Fine";
        if (accruedFine > 0.0) {
            QString formatted = vn.toString(static_cast<qulonglong>(accruedFine)) + " VND";
            fineStatusText = (rec.finePaid >= accruedFine)
                                 ? QString("Paid (%1)").arg(formatted)
                                 : QString("Unpaid (%1)").arg(formatted);
        }
        m_borrowTable->setItem(row, 7, new QTableWidgetItem(fineStatusText));
    }
}

void MainWindow::refreshReservationTable() {
    m_reservationTable->setRowCount(0);
    bool isMember = (m_currentUser.role == UserRole::Member);

    for (const auto& res : m_reservations) {
        if (isMember && res.memberId != m_currentUser.memberId) {
            continue;
        }
        int row = m_reservationTable->rowCount();
        m_reservationTable->insertRow(row);
        m_reservationTable->setItem(row, 0, new QTableWidgetItem(res.reserveId));
        m_reservationTable->setItem(row, 1, new QTableWidgetItem(res.memberId));
        m_reservationTable->setItem(row, 2, new QTableWidgetItem(res.isbn));
        m_reservationTable->setItem(row, 3, new QTableWidgetItem(res.bookTitle));
        m_reservationTable->setItem(row, 4, new QTableWidgetItem(res.reserveDate.toString("dd/MM/yyyy")));
        m_reservationTable->setItem(row, 5, new QTableWidgetItem(res.notes));
    }
}

void MainWindow::refreshReportsTab() {
    int totalBooksCount = 0;
    for (const auto& b : m_books) totalBooksCount += b.totalCopies;

    int activeBorrows = 0;
    int overdueCount = 0;
    double totalFines = 0.0;

    for (const auto& r : m_borrows) {
        totalFines += r.finePaid;
        if (!r.isReturned) {
            activeBorrows++;
            if (r.dueDate < QDate::currentDate()) overdueCount++;
        }
    }

    QLocale vn(QLocale::Vietnamese);
    m_statTotalBooks->setText(QString("<b>Total Copies:</b> %1").arg(totalBooksCount));
    m_statActiveBorrows->setText(QString("<b>Active Borrows:</b> %1").arg(activeBorrows));
    m_statOverdueCount->setText(QString("<b>Overdue Loans:</b> <span style='color:#d9534f;'>%1</span>").arg(overdueCount));
    m_statTotalFines->setText(QString("<b>Collected Fines:</b> %1 VND").arg(vn.toString(static_cast<qlonglong>(totalFines))));

    m_usersTable->setRowCount(0);
    for (const auto& u : m_users) {
        int row = m_usersTable->rowCount();
        m_usersTable->insertRow(row);
        m_usersTable->setItem(row, 0, new QTableWidgetItem(u.username));
        m_usersTable->setItem(row, 1, new QTableWidgetItem(roleToString(u.role)));
        m_usersTable->setItem(row, 2, new QTableWidgetItem(u.memberId));
    }
}

void MainWindow::onSearchTextChanged(const QString& text) {
    refreshBookTable(text);
}

void MainWindow::onBorrowBookClicked() {
    int row = m_bookTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Book", "Please select a book from the catalog first.");
        return;
    }

    QString isbn = m_bookTable->item(row, 0)->text();
    for (auto& book : m_books) {
        if (book.isbn == isbn) {
            if (book.availableCopies <= 0) {
                QMessageBox::warning(this, "Unavailable",
                                     "This book has 0 available copies. Please use 'Reserve Book' instead.");
                return;
            }
            QString defaultId = (m_currentUser.role == UserRole::Member) ? m_currentUser.memberId : "";
            BorrowDialog dlg(book, defaultId, this);
            if (dlg.exec() == QDialog::Accepted) {
                book.availableCopies--;
                QString recId = QString("BRW-%1").arg(m_borrows.size() + 1, 3, 10, QChar('0'));
                m_borrows.append({recId, dlg.getMemberId(), book.isbn, book.title,
                                  dlg.getBorrowDate(), dlg.getDueDate(), false, 0.0});

                refreshBookTable(m_searchEdit->text());
                refreshBorrowTable();
                refreshReportsTab();
                QMessageBox::information(this, "Success", "Book borrowed successfully!");
            }
            return;
        }
    }
}

void MainWindow::onReserveBookClicked() {
    int row = m_bookTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Book", "Please select an unavailable book to reserve.");
        return;
    }

    QString isbn = m_bookTable->item(row, 0)->text();
    for (const auto& book : m_books) {
        if (book.isbn == isbn) {
            if (book.availableCopies > 0) {
                QMessageBox::information(this, "Book Available",
                                         "This book is currently in stock. You can borrow it directly via 'Borrow Book'.");
                return;
            }
            QString defaultId = (m_currentUser.role == UserRole::Member) ? m_currentUser.memberId : "";
            ReserveDialog dlg(book, defaultId, this);
            if (dlg.exec() == QDialog::Accepted) {
                QString resId = QString("RES-%1").arg(m_reservations.size() + 1, 3, 10, QChar('0'));
                m_reservations.append({resId, dlg.getMemberId(), book.isbn, book.title,
                                       dlg.getReserveDate(), dlg.getNotes()});
                refreshReservationTable();
                QMessageBox::information(this, "Reserved", "Reservation request submitted successfully!");
            }
            return;
        }
    }
}

void MainWindow::onReturnAndFineClicked() {
    int row = m_borrowTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Transaction", "Please select an active borrow record to return.");
        return;
    }

    QString recId = m_borrowTable->item(row, 0)->text();
    for (auto& rec : m_borrows) {
        if (rec.recordId == recId) {
            if (rec.isReturned) {
                QMessageBox::information(this, "Already Returned", "This book has already been returned.");
                return;
            }
            ReturnFineDialog dlg(rec, this);
            if (dlg.exec() == QDialog::Accepted) {
                double calculatedFine = dlg.getCalculatedFine();
                rec.isReturned = true;
                rec.returnDate = dlg.getReturnDate();
                rec.fineAmount = calculatedFine;

                // Cộng lại số lượng sách khả dụng (FR5)
                for (auto& book : m_books) {
                    if (book.isbn == rec.isbn) {
                        book.availableCopies++;
                        break;
                    }
                }

                // Kiểm tra hàng đợi đặt trước đầu tiên theo FIFO (FR6, FR9)
                QString notifiedMember;
                for (int i = 0; i < m_reservations.size(); ++i) {
                    if (m_reservations[i].isbn == rec.isbn) {
                        notifiedMember = m_reservations[i].memberId;
                        m_reservations.removeAt(i);
                        break;
                    }
                }

                refreshBookTable(m_searchEdit->text());
                refreshBorrowTable();
                refreshReservationTable();
                refreshReportsTab();

                QLocale vn(QLocale::Vietnamese);
                QString msg = "Book return confirmed.";
                if (calculatedFine > 0.0) {
                    QString formattedFine = vn.toString(static_cast<qulonglong>(calculatedFine));
                    msg += QString("\nAccrued fine: %1 VND (Status: Unpaid - click 'Mark Fine Paid' once collected).")
                            .arg(formattedFine);
                }
                if (!notifiedMember.isEmpty()) {
                    msg += QString("\nNotification sent to %1: Reserved book is now available for pickup.")
                            .arg(notifiedMember);
                }
                QMessageBox::information(this, "Returned", msg);
            }
            return;
        }
    }
}

void MainWindow::onMarkFinePaidClicked() {
    int row = m_borrowTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Select Transaction", "Please select a transaction to record fine payment.");
        return;
    }

    QString recId = m_borrowTable->item(row, 0)->text();
    for (auto& rec : m_borrows) {
        if (rec.recordId == recId) {
            if (!rec.isReturned) {
                QMessageBox::warning(this, "Book Not Returned",
                                     "Please process the book return first using 'Return Book & Calculate Fine'.");
                return;
            }
            if (rec.fineAmount <= 0.0) {
                QMessageBox::information(this, "No Fine", "This transaction has no overdue fine.");
                return;
            }
            if (rec.finePaid >= rec.fineAmount) {
                QMessageBox::information(this, "Already Paid", "This fine has already been marked as Paid.");
                return;
            }

            rec.finePaid = rec.fineAmount;
            refreshBorrowTable();
            refreshReportsTab();

            QLocale vn(QLocale::Vietnamese);
            QMessageBox::information(this, "Fine Payment Recorded",
                                     QString("Collected %1 VND for %2. Status updated to Paid.")
                                         .arg(vn.toString(static_cast<qlonglong>(rec.finePaid)), rec.recordId));
            return;
        }
    }
}