#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QVector>
#include "LibraryModels.h"

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool connectToDatabase(const QString& host,
                           int port,
                           const QString& dbName,
                           const QString& userName,
                           const QString& password,
                           QString* errorMessage = nullptr);

    bool isOpen() const;

    bool loadAllData(QVector<User>& users,
                     QVector<Book>& books,
                     QVector<BorrowRecord>& borrows,
                     QVector<ReservationRecord>& reservations,
                     QString* errorMessage = nullptr);

    bool insertUser(const User& user, QString* errorMessage = nullptr);
    bool insertBorrow(const BorrowRecord& record, int availableAfterBorrow, QString* errorMessage = nullptr);
    bool insertReservation(const ReservationRecord& res, QString* errorMessage = nullptr);
    bool recordBookReturn(const BorrowRecord& record,
                          int availableAfterReturn,
                          const QString* notifiedReserveCode,
                          const QString* notifiedMemberId,
                          const QString& bookTitle,
                          QString* errorMessage = nullptr);
    bool markFinePaid(const QString& recordCode, double paidAmount, QString* errorMessage = nullptr);
    bool cancelReservation(const QString& reserveCode, QString* errorMessage = nullptr);
    bool updateUserRole(const QString& username, UserRole newRole, QString* errorMessage = nullptr);
    bool removeUser(const QString& username, QString* errorMessage = nullptr);

private:
    QSqlDatabase m_db;
    int getUserIdByMemberOrName(const QString& memberIdOrUsername) const;
    int getBookIdByIsbn(const QString& isbn) const;
    int getTransactionIdByCode(const QString& recordCode) const;
    static QString roleToDbString(UserRole role);
    static UserRole roleFromDbString(const QString& roleStr);
};

#endif // DATABASEMANAGER_H