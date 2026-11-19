#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include "LibraryModels.h"

// 1. Màn hình Mượn Sách (Borrow Book Screen)
class BorrowDialog : public QDialog {
    Q_OBJECT
public:
    explicit BorrowDialog(const Book& book, const QString& defaultMemberId, QWidget* parent = nullptr);

    QString getMemberId() const;
    QDate getBorrowDate() const;
    QDate getDueDate() const;

private slots:
    void onConfirmBorrow();

private:
    Book m_book;
    QLineEdit* m_memberIdEdit;
    QDateEdit* m_borrowDateEdit;
    QDateEdit* m_dueDateEdit;
};

// 2. Màn hình Đặt Trước Sách (Reserve Book Screen)
class ReserveDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReserveDialog(const Book& book, const QString& defaultMemberId, QWidget* parent = nullptr);

    QString getMemberId() const;
    QDate getReserveDate() const;
    QString getNotes() const;

private slots:
    void onSubmitRequest();

private:
    Book m_book;
    QLineEdit* m_memberIdEdit;
    QDateEdit* m_reserveDateEdit;
    QTextEdit* m_notesEdit;
};

// 3. Màn hình Trả Sách & Đóng Phạt (Return Book & Fine Screen)
class ReturnFineDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReturnFineDialog(const BorrowRecord& record, QWidget* parent = nullptr);

    double getCalculatedFine() const { return m_totalFine; }
    int getOverdueDays() const { return m_overdueDays; }

private slots:
    void recalculateFine();
    void onConfirmReturn();

private:
    BorrowRecord m_record;
    QDateEdit* m_returnDateEdit;
    QLabel* m_overdueDaysLabel;
    QLabel* m_totalFineLabel;
    int m_overdueDays = 0;
    double m_totalFine = 0.0;
    const double FINE_PER_DAY = 5000.0; // 5.000 VNĐ / ngày quá hạn
};

#endif // DIALOGS_H