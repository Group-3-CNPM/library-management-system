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
#include <QFileDialog>
#include <QTextStream>

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
                                         .arg(vn.toString(static_cast<qulonglong>(rec.finePaid)), rec.recordId));
            return;
        }
    }
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
    auto* catalogLayout = new QVBoxLayout(catalogTab); // <-- Gán trực tiếp catalogTab tại đây

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

    auto* reportControlsLayout = new QHBoxLayout();
    m_reportTypeCombo = new QComboBox();
    m_reportTypeCombo->addItems({
        "Borrowing Report",
        "Return Report",
        "Overdue Books Report",
        "Fine Collection Report",
        "Member Activity Report",
        "Inventory Summary"
    });
    m_reportFilterEdit = new QLineEdit();
    m_reportFilterEdit->setPlaceholderText("Filter by Member ID, ISBN, Title, or Status...");
    m_exportReportBtn = new QPushButton("Export Report (CSV / HTML)");
    m_exportReportBtn->setObjectName("primaryButton");

    reportControlsLayout->addWidget(new QLabel("Report Type:"));
    reportControlsLayout->addWidget(m_reportTypeCombo);
    reportControlsLayout->addWidget(m_reportFilterEdit);
    reportControlsLayout->addWidget(m_exportReportBtn);

    m_reportTable = new QTableWidget(0, 5);
    m_reportTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_adminUsersGroup = new QWidget();
    auto* adminUsersLayout = new QVBoxLayout(m_adminUsersGroup);
    adminUsersLayout->setContentsMargins(0, 6, 0, 0);

    auto* userToolbarLayout = new QHBoxLayout();
    userToolbarLayout->addWidget(new QLabel("<b>System Accounts & Role Assignments (Administrator)</b>"));
    userToolbarLayout->addStretch();
    m_addUserBtn = new QPushButton("Add Account");
    m_toggleRoleBtn = new QPushButton("Cycle Role");
    m_removeUserBtn = new QPushButton("Remove Account");
    userToolbarLayout->addWidget(m_addUserBtn);
    userToolbarLayout->addWidget(m_toggleRoleBtn);
    userToolbarLayout->addWidget(m_removeUserBtn);

    m_usersTable = new QTableWidget(0, 3);
    m_usersTable->setHorizontalHeaderLabels({"Username", "Role", "Member / Staff ID"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    adminUsersLayout->addLayout(userToolbarLayout);
    adminUsersLayout->addWidget(m_usersTable);

    adminLayout->addWidget(statsGroup);
    adminLayout->addLayout(reportControlsLayout);
    adminLayout->addWidget(m_reportTable);
    adminLayout->addWidget(m_adminUsersGroup);

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
    connect(m_markPaidBtn, &QPushButton::clicked, this, &MainWindow::onMarkFinePaidClicked);
    connect(m_cancelReserveBtn, &QPushButton::clicked, this, &MainWindow::onCancelReservationClicked);
    connect(m_reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onReportTypeChanged);
    connect(m_reportFilterEdit, &QLineEdit::textChanged,
            this, [this](const QString&) { refreshReportsTab(); });
    connect(m_exportReportBtn, &QPushButton::clicked, this, &MainWindow::onExportReportClicked);
    connect(m_addUserBtn, &QPushButton::clicked, this, &MainWindow::onAddUserClicked);
    connect(m_toggleRoleBtn, &QPushButton::clicked, this, &MainWindow::onToggleUserRoleClicked);
    connect(m_removeUserBtn, &QPushButton::clicked, this, &MainWindow::onRemoveUserClicked);
    return dashboard;
}

void MainWindow::applyRolePermissions() {
    if (!m_dashboardTabs) return;

    bool isMember = (m_currentUser.role == UserRole::Member);
    bool isAdmin  = (m_currentUser.role == UserRole::Administrator);

    m_dashboardTabs->setTabEnabled(0, true);
    m_dashboardTabs->setTabEnabled(1, true);
    m_dashboardTabs->setTabEnabled(2, !isMember);

    if (m_markPaidBtn) {
        m_markPaidBtn->setVisible(!isMember);
    }
    if (m_returnFineBtn) {
        m_returnFineBtn->setText(isMember ? "Request Return / View Fine" : "Return Book & Calculate Fine");
    }
    if (m_adminUsersGroup) {
        m_adminUsersGroup->setVisible(isAdmin);
    }
    if (m_usersTable) {
        m_usersTable->setVisible(isAdmin);
    }

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
    double totalCollectedFines = 0.0;

    for (const auto& r : m_borrows) {
        totalCollectedFines += r.finePaid;
        if (!r.isReturned) {
            activeBorrows++;
            if (r.dueDate < QDate::currentDate()) overdueCount++;
        }
    }

    QLocale vn(QLocale::Vietnamese);
    m_statTotalBooks->setText(QString("<b>Total Copies:</b> %1").arg(totalBooksCount));
    m_statActiveBorrows->setText(QString("<b>Active Borrows:</b> %1").arg(activeBorrows));
    m_statOverdueCount->setText(QString("<b>Overdue Loans:</b> <span style='color:#d9534f;'>%1</span>").arg(overdueCount));
    m_statTotalFines->setText(QString("<b>Collected Fines:</b> %1 VND").arg(vn.toString(static_cast<qulonglong>(totalCollectedFines))));

    // Cập nhật bảng báo cáo chi tiết theo loại & bộ lọc (FR10, FR11, UC-15)
    QString filter = m_reportFilterEdit ? m_reportFilterEdit->text().trimmed() : "";
    int reportIdx = m_reportTypeCombo ? m_reportTypeCombo->currentIndex() : 0;

    auto matchesFilter = [&](const QString& s1, const QString& s2 = "", const QString& s3 = "") -> bool {
        if (filter.isEmpty()) return true;
        return s1.contains(filter, Qt::CaseInsensitive) ||
               s2.contains(filter, Qt::CaseInsensitive) ||
               s3.contains(filter, Qt::CaseInsensitive);
    };

    if (m_reportTable) {
        m_reportTable->setRowCount(0);

        if (reportIdx == 0) { // Borrowing Report
            m_reportTable->setColumnCount(5);
            m_reportTable->setHorizontalHeaderLabels({"Record ID", "Member ID", "Book Title", "Borrow Date", "Due Date"});
            for (const auto& r : m_borrows) {
                if (!matchesFilter(r.recordId, r.memberId, r.bookTitle)) continue;
                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(r.recordId));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(r.memberId));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(r.bookTitle));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(r.borrowDate.toString("dd/MM/yyyy")));
                m_reportTable->setItem(row, 4, new QTableWidgetItem(r.dueDate.toString("dd/MM/yyyy")));
            }
        } else if (reportIdx == 1) { // Return Report
            m_reportTable->setColumnCount(4);
            m_reportTable->setHorizontalHeaderLabels({"Record ID", "Member ID", "Book Title", "Return Status"});
            for (const auto& r : m_borrows) {
                QString st = r.isReturned ? "Returned" : "Not Returned";
                if (!matchesFilter(r.recordId, r.memberId, r.bookTitle + " " + st)) continue;
                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(r.recordId));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(r.memberId));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(r.bookTitle));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(st));
            }
        } else if (reportIdx == 2) { // Overdue Books Report
            m_reportTable->setColumnCount(5);
            m_reportTable->setHorizontalHeaderLabels({"Record ID", "Member ID", "Book Title", "Overdue Days", "Estimated Fine (5,000/d)"});
            for (const auto& r : m_borrows) {
                int days = (!r.isReturned && r.dueDate < QDate::currentDate())
                               ? r.dueDate.daysTo(QDate::currentDate())
                               : 0;
                if (days <= 0) continue;
                if (!matchesFilter(r.recordId, r.memberId, r.bookTitle)) continue;
                qulonglong estFine = static_cast<qulonglong>(days) * 5000ULL;
                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(r.recordId));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(r.memberId));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(r.bookTitle));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(QString::number(days)));
                m_reportTable->setItem(row, 4, new QTableWidgetItem(vn.toString(estFine) + " VND"));
            }
        } else if (reportIdx == 3) { // Fine Collection Report
            m_reportTable->setColumnCount(4);
            m_reportTable->setHorizontalHeaderLabels({"Record ID", "Member ID", "Book Title", "Collected Fine"});
            for (const auto& r : m_borrows) {
                if (!matchesFilter(r.recordId, r.memberId, r.bookTitle)) continue;
                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(r.recordId));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(r.memberId));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(r.bookTitle));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(vn.toString(static_cast<qulonglong>(r.finePaid)) + " VND"));
            }
        } else if (reportIdx == 4) { // Member Activity Report
            m_reportTable->setColumnCount(4);
            m_reportTable->setHorizontalHeaderLabels({"Member ID", "Username", "Borrow Count", "Reservation Count"});
            for (const auto& u : m_users) {
                if (!matchesFilter(u.memberId, u.username, roleToString(u.role))) continue;
                int bCount = 0;
                int rCount = 0;
                for (const auto& br : m_borrows) if (br.memberId == u.memberId) bCount++;
                for (const auto& rs : m_reservations) if (rs.memberId == u.memberId) rCount++;

                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(u.memberId));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(u.username));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(QString::number(bCount)));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(QString::number(rCount)));
            }
        } else { // Inventory Summary (FR11)
            m_reportTable->setColumnCount(5);
            m_reportTable->setHorizontalHeaderLabels({"ISBN", "Title", "Author", "Available", "Total Copies"});
            for (const auto& b : m_books) {
                if (!matchesFilter(b.isbn, b.title, b.author)) continue;
                int row = m_reportTable->rowCount();
                m_reportTable->insertRow(row);
                m_reportTable->setItem(row, 0, new QTableWidgetItem(b.isbn));
                m_reportTable->setItem(row, 1, new QTableWidgetItem(b.title));
                m_reportTable->setItem(row, 2, new QTableWidgetItem(b.author));
                m_reportTable->setItem(row, 3, new QTableWidgetItem(QString::number(b.availableCopies)));
                m_reportTable->setItem(row, 4, new QTableWidgetItem(QString::number(b.totalCopies)));
            }
        }
    }

    if (m_usersTable) {
        m_usersTable->setRowCount(0);
        for (const auto& u : m_users) {
            int row = m_usersTable->rowCount();
            m_usersTable->insertRow(row);
            m_usersTable->setItem(row, 0, new QTableWidgetItem(u.username));
            m_usersTable->setItem(row, 1, new QTableWidgetItem(roleToString(u.role)));
            m_usersTable->setItem(row, 2, new QTableWidgetItem(u.memberId));
        }
    }
}

void MainWindow::onReportTypeChanged(int) {
    refreshReportsTab();
}

void MainWindow::onExportReportClicked() {
    if (!m_reportTable || m_reportTable->rowCount() == 0) {
        QMessageBox::information(this, "Empty Report", "There are no report rows to export.");
        return;
    }

    QString suggestedName = m_reportTypeCombo->currentText().toLower().replace(' ', '_') + ".csv";
    QString filePath = QFileDialog::getSaveFileName(
        this, "Export Report", suggestedName,
        "CSV / Excel Files (*.csv);;HTML Report (*.html);;Text Report (*.txt)"
    );
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Failed", "Could not write to the selected file.");
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#endif

    int cols = m_reportTable->columnCount();
    int rows = m_reportTable->rowCount();

    if (filePath.endsWith(".html", Qt::CaseInsensitive)) {
        out << "<html><head><meta charset='utf-8'><title>" << m_reportTypeCombo->currentText()
            << "</title></head><body><h2>" << m_reportTypeCombo->currentText()
            << "</h2><table border='1' cellpadding='6' cellspacing='0'><tr>";
        for (int c = 0; c < cols; ++c) {
            out << "<th>" << m_reportTable->horizontalHeaderItem(c)->text().toHtmlEscaped() << "</th>";
        }
        out << "</tr>\n";
        for (int r = 0; r < rows; ++r) {
            out << "<tr>";
            for (int c = 0; c < cols; ++c) {
                QString val = m_reportTable->item(r, c) ? m_reportTable->item(r, c)->text() : "";
                out << "<td>" << val.toHtmlEscaped() << "</td>";
            }
            out << "</tr>\n";
        }
        out << "</table></body></html>\n";
    } else {
        QString sep = filePath.endsWith(".txt", Qt::CaseInsensitive) ? "\t" : ",";
        for (int c = 0; c < cols; ++c) {
            QString h = m_reportTable->horizontalHeaderItem(c)->text().replace("\"", "\"\"");
            out << "\"" << h << "\"" << (c + 1 < cols ? sep : "");
        }
        out << "\n";
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                QString val = m_reportTable->item(r, c) ? m_reportTable->item(r, c)->text() : "";
                val.replace("\"", "\"\"");
                out << "\"" << val << "\"" << (c + 1 < cols ? sep : "");
            }
            out << "\n";
        }
    }

    file.close();
    QMessageBox::information(this, "Export Successful",
                             QString("Report exported successfully to:\n%1").arg(filePath));
}

void MainWindow::onAddUserClicked() {
    if (m_currentUser.role != UserRole::Administrator) {
        QMessageBox::warning(this, "Access Denied", "Only Administrators can add user accounts.");
        return;
    }

    bool ok = false;
    QString uname = QInputDialog::getText(this, "Add User Account", "Username:", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || uname.isEmpty()) return;

    for (const auto& u : m_users) {
        if (u.username.compare(uname, Qt::CaseInsensitive) == 0) {
            QMessageBox::warning(this, "Duplicate User", "Username already exists.");
            return;
        }
    }

    QString pwd = QInputDialog::getText(this, "Add User Account", "Password:", QLineEdit::Password, "123", &ok);
    if (!ok || pwd.isEmpty()) return;

    QStringList roles = {"Member", "Librarian", "Administrator"};
    QString chosenRole = QInputDialog::getItem(this, "Add User Account", "Select Role:", roles, 0, false, &ok);
    if (!ok) return;

    UserRole role = UserRole::Member;
    QString prefix = "MEM";
    if (chosenRole == "Librarian") {
        role = UserRole::Librarian;
        prefix = "LIB";
    } else if (chosenRole == "Administrator") {
        role = UserRole::Administrator;
        prefix = "ADM";
    }

    QString codeId = QString("%1-%2").arg(prefix).arg(100 + m_users.size() + 1);
    m_users.append({uname, pwd, role, codeId});
    refreshReportsTab();
    QMessageBox::information(this, "Account Added",
                             QString("Created account '%1' (%2) with ID %3.").arg(uname, chosenRole, codeId));
}

void MainWindow::onToggleUserRoleClicked() {
    if (m_currentUser.role != UserRole::Administrator) {
        QMessageBox::warning(this, "Access Denied", "Only Administrators can change user roles.");
        return;
    }

    int row = m_usersTable->currentRow();
    if (row < 0 || row >= m_users.size()) {
        QMessageBox::information(this, "Select Account", "Please select a user account first.");
        return;
    }

    auto& target = m_users[row];
    if (target.role == UserRole::Member) {
        target.role = UserRole::Librarian;
    } else if (target.role == UserRole::Librarian) {
        target.role = UserRole::Administrator;
    } else {
        int adminCount = 0;
        for (const auto& u : m_users) if (u.role == UserRole::Administrator) adminCount++;
        if (adminCount <= 1) {
            QMessageBox::warning(this, "Last Administrator",
                                 "Cannot demote the last Administrator account in the system.");
            return;
        }
        target.role = UserRole::Member;
    }

    refreshReportsTab();
    QMessageBox::information(this, "Role Updated",
                             QString("Updated role of '%1' to %2.")
                                 .arg(target.username, roleToString(target.role)));
}

void MainWindow::onRemoveUserClicked() {
    if (m_currentUser.role != UserRole::Administrator) {
        QMessageBox::warning(this, "Access Denied", "Only Administrators can remove user accounts.");
        return;
    }

    int row = m_usersTable->currentRow();
    if (row < 0 || row >= m_users.size()) {
        QMessageBox::information(this, "Select Account", "Please select a user account to remove.");
        return;
    }

    const auto& target = m_users[row];
    if (target.username == m_currentUser.username) {
        QMessageBox::warning(this, "Action Blocked", "You cannot remove the currently logged-in account.");
        return;
    }
    if (target.role == UserRole::Administrator) {
        int adminCount = 0;
        for (const auto& u : m_users) if (u.role == UserRole::Administrator) adminCount++;
        if (adminCount <= 1) {
            QMessageBox::warning(this, "Last Administrator",
                                 "Cannot remove the last Administrator account.");
            return;
        }
    }

    QString removedName = target.username;
    m_users.removeAt(row);
    refreshReportsTab();
    QMessageBox::information(this, "Account Removed",
                             QString("Account '%1' has been removed.").arg(removedName));
}
