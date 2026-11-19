#include "Dialogs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QLocale>
#include <QtGlobal>

// ============================================================================
// 1. BORROW BOOK SCREEN
// ============================================================================
BorrowDialog::BorrowDialog(const Book& book, const QString& defaultMemberId, QWidget* parent)
    : QDialog(parent), m_book(book) {
    setWindowTitle("Borrow Book");
    setMinimumWidth(420);

    auto* mainLayout = new QVBoxLayout(this);

    // Nhóm thông tin sách được chọn
    auto* bookGroup = new QGroupBox("Selected Book Information", this);
    auto* bookForm = new QFormLayout(bookGroup);
    bookForm->addRow("Title:", new QLabel("<b>" + m_book.title + "</b>"));
    bookForm->addRow("Author:", new QLabel(m_book.author));
    bookForm->addRow("ISBN:", new QLabel(m_book.isbn));
    bookForm->addRow("Available Copies:", new QLabel(QString::number(m_book.availableCopies)));

    // Nhóm thông tin người mượn
    auto* inputGroup = new QGroupBox("Borrow Details", this);
    auto* inputForm = new QFormLayout(inputGroup);

    m_memberIdEdit = new QLineEdit(defaultMemberId, this);
    m_memberIdEdit->setPlaceholderText("Enter Member ID (e.g., MEM-001)");

    m_borrowDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_borrowDateEdit->setCalendarPopup(true);
    m_borrowDateEdit->setDisplayFormat("dd/MM/yyyy");

    m_dueDateEdit = new QDateEdit(QDate::currentDate().addDays(14), this);
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDisplayFormat("dd/MM/yyyy");

    inputForm->addRow("Member ID:", m_memberIdEdit);
    inputForm->addRow("Borrow Date:", m_borrowDateEdit);
    inputForm->addRow("Due Date:", m_dueDateEdit);

    // Nút bấm
    auto* btnLayout = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    auto* confirmBtn = new QPushButton("Confirm Borrow", this);
    confirmBtn->setObjectName("primaryButton");

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);

    mainLayout->addWidget(bookGroup);
    mainLayout->addWidget(inputGroup);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, this, &BorrowDialog::onConfirmBorrow);
}

QString BorrowDialog::getMemberId() const { return m_memberIdEdit->text().trimmed(); }
QDate BorrowDialog::getBorrowDate() const { return m_borrowDateEdit->date(); }
QDate BorrowDialog::getDueDate() const { return m_dueDateEdit->date(); }

void BorrowDialog::onConfirmBorrow() {
    if (getMemberId().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid Member ID.");
        return;
    }
    if (getDueDate() < getBorrowDate()) {
        QMessageBox::warning(this, "Validation Error", "Due Date cannot be earlier than Borrow Date.");
        return;
    }
    accept();
}

// ============================================================================
// 2. RESERVE BOOK SCREEN
// ============================================================================
ReserveDialog::ReserveDialog(const Book& book, const QString& defaultMemberId, QWidget* parent)
    : QDialog(parent), m_book(book) {
    setWindowTitle("Reserve Book");
    setMinimumWidth(420);

    auto* mainLayout = new QVBoxLayout(this);

    // Thông tin sách hiện không có sẵn
    auto* bookGroup = new QGroupBox("Unavailable Book Information", this);
    auto* bookForm = new QFormLayout(bookGroup);
    bookForm->addRow("Title:", new QLabel("<b>" + m_book.title + "</b>"));
    bookForm->addRow("Author:", new QLabel(m_book.author));
    bookForm->addRow("ISBN:", new QLabel(m_book.isbn));
    auto* statusLbl = new QLabel("<span style='color:#d9534f; font-weight:bold;'>Out of Stock (0 available)</span>");
    bookForm->addRow("Status:", statusLbl);

    // Form đặt trước
    auto* formGroup = new QGroupBox("Reservation Request", this);
    auto* formLayout = new QFormLayout(formGroup);

    m_memberIdEdit = new QLineEdit(defaultMemberId, this);
    m_memberIdEdit->setPlaceholderText("Enter Member ID (e.g., MEM-001)");

    m_reserveDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_reserveDateEdit->setCalendarPopup(true);
    m_reserveDateEdit->setDisplayFormat("dd/MM/yyyy");

    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setPlaceholderText("Optional notes (e.g., preferred pickup branch or urgency)...");
    m_notesEdit->setMaximumHeight(80);

    formLayout->addRow("Member ID:", m_memberIdEdit);
    formLayout->addRow("Reserve Date:", m_reserveDateEdit);
    formLayout->addRow("Notes:", m_notesEdit);

    // Nút Submit Request & Cancel
    auto* btnLayout = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    auto* submitBtn = new QPushButton("Submit Request", this);
    submitBtn->setObjectName("primaryButton");

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(submitBtn);

    mainLayout->addWidget(bookGroup);
    mainLayout->addWidget(formGroup);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(submitBtn, &QPushButton::clicked, this, &ReserveDialog::onSubmitRequest);
}

QString ReserveDialog::getMemberId() const { return m_memberIdEdit->text().trimmed(); }
QDate ReserveDialog::getReserveDate() const { return m_reserveDateEdit->date(); }
QString ReserveDialog::getNotes() const { return m_notesEdit->toPlainText().trimmed(); }

void ReserveDialog::onSubmitRequest() {
    if (getMemberId().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Member ID is required to submit a reservation.");
        return;
    }
    accept();
}

// ============================================================================
// 3. RETURN BOOK & FINE SCREEN
// ============================================================================
ReturnFineDialog::ReturnFineDialog(const BorrowRecord& record, QWidget* parent)
    : QDialog(parent), m_record(record) {
    setWindowTitle("Return Book & Fine Calculation");
    setMinimumWidth(440);

    auto* mainLayout = new QVBoxLayout(this);

    // Chi tiết giao dịch mượn
    auto* detailGroup = new QGroupBox("Borrow Transaction Details", this);
    auto* detailForm = new QFormLayout(detailGroup);
    detailForm->addRow("Transaction ID:", new QLabel(m_record.recordId));
    detailForm->addRow("Member ID:", new QLabel(m_record.memberId));
    detailForm->addRow("Book Title:", new QLabel("<b>" + m_record.bookTitle + "</b>"));
    detailForm->addRow("Borrow Date:", new QLabel(m_record.borrowDate.toString("dd/MM/yyyy")));
    detailForm->addRow("Due Date:", new QLabel(m_record.dueDate.toString("dd/MM/yyyy")));

    // Tính toán quá hạn & tiền phạt
    auto* fineGroup = new QGroupBox("Return & Automatic Fine Calculation", this);
    auto* fineForm = new QFormLayout(fineGroup);

    m_returnDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_returnDateEdit->setCalendarPopup(true);
    m_returnDateEdit->setDisplayFormat("dd/MM/yyyy");

    m_overdueDaysLabel = new QLabel(this);
    m_totalFineLabel = new QLabel(this);

    fineForm->addRow("Actual Return Date:", m_returnDateEdit);
    fineForm->addRow("Fine Rate:", new QLabel("5,000 VND / overdue day"));
    fineForm->addRow("Overdue Days:", m_overdueDaysLabel);
    fineForm->addRow("Total Fine:", m_totalFineLabel);

    // Nút Confirm Return
    auto* btnLayout = new QHBoxLayout();
    auto* cancelBtn = new QPushButton("Cancel", this);
    auto* confirmReturnBtn = new QPushButton("Confirm Return", this);
    confirmReturnBtn->setObjectName("successButton");

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmReturnBtn);

    mainLayout->addWidget(detailGroup);
    mainLayout->addWidget(fineGroup);
    mainLayout->addLayout(btnLayout);

    recalculateFine();

    connect(m_returnDateEdit, &QDateEdit::dateChanged, this, &ReturnFineDialog::recalculateFine);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(confirmReturnBtn, &QPushButton::clicked, this, &ReturnFineDialog::onConfirmReturn);
}

void ReturnFineDialog::recalculateFine() {
    QDate returnDate = m_returnDateEdit->date();
    int diff = m_record.dueDate.daysTo(returnDate);
    m_overdueDays = qMax(0, diff);
    m_totalFine = m_overdueDays * FINE_PER_DAY;

    QLocale vnLocale(QLocale::Vietnamese);
    QString formattedFine = vnLocale.toString(static_cast<qlonglong>(m_totalFine)) + " VND";

    if (m_overdueDays > 0) {
        m_overdueDaysLabel->setText(QString("<span style='color:#d9534f; font-weight:bold;'>%1 days</span>").arg(m_overdueDays));
        m_totalFineLabel->setText(QString("<span style='color:#d9534f; font-size:15px; font-weight:bold;'>%1</span>").arg(formattedFine));
    } else {
        m_overdueDaysLabel->setText("<span style='color:#28a745; font-weight:bold;'>0 days (On time)</span>");
        m_totalFineLabel->setText("<span style='color:#28a745; font-size:15px; font-weight:bold;'>0 VND</span>");
    }
}

void ReturnFineDialog::onConfirmReturn() {
    accept();
}