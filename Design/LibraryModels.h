#ifndef LIBRARYMODELS_H
#define LIBRARYMODELS_H

#include <QString>
#include <QDate>
#include <QVector>

enum class UserRole {
    Member,
    Librarian,
    Administrator
};

inline QString roleToString(UserRole role) {
    switch (role) {
        case UserRole::Member: return "Member";
        case UserRole::Librarian: return "Librarian";
        case UserRole::Administrator: return "Administrator";
    }
    return "Unknown";
}

struct User {
    QString username;
    QString password;
    UserRole role;
    QString memberId;
};

struct Book {
    QString isbn;
    QString title;
    QString author;
    int totalCopies;
    int availableCopies;
};

struct BorrowRecord {
    QString recordId;
    QString memberId;
    QString isbn;
    QString bookTitle;
    QDate borrowDate;
    QDate dueDate;
    bool isReturned = false;
    double finePaid = 0.0;
    double fineAmount = 0.0;
    QDate returnDate;
};

struct ReservationRecord {
    QString reserveId;
    QString memberId;
    QString isbn;
    QString bookTitle;
    QDate reserveDate;
    QString notes;
};

#endif // LIBRARYMODELS_H