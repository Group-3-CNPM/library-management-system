#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("LibraryMySqlConn")) {
        m_db = QSqlDatabase::database("LibraryMySqlConn");
    } else {
        m_db = QSqlDatabase::addDatabase("QMYSQL", "LibraryMySqlConn");
    }
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::connectToDatabase(const QString& host,
                                        int port,
                                        const QString& dbName,
                                        const QString& userName,
                                        const QString& password,
                                        QString* errorMessage) {
    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        if (errorMessage) {
            *errorMessage = "Qt driver 'QMYSQL' is not available in this environment.";
        }
        return false;
    }

    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(userName);
    m_db.setPassword(password);

    if (!m_db.open()) {
        if (errorMessage) {
            *errorMessage = m_db.lastError().text();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

QString DatabaseManager::roleToDbString(UserRole role) {
    switch (role) {
    case UserRole::Administrator: return "Administrator";
    case UserRole::Librarian:     return "Librarian";
    case UserRole::Member:
    default:                      return "Member";
    }
}

UserRole DatabaseManager::roleFromDbString(const QString& roleStr) {
    if (roleStr.compare("Administrator", Qt::CaseInsensitive) == 0) return UserRole::Administrator;
    if (roleStr.compare("Librarian", Qt::CaseInsensitive) == 0)     return UserRole::Librarian;
    return UserRole::Member;
}

int DatabaseManager::getUserIdByMemberOrName(const QString& memberIdOrUsername) const {
    QSqlQuery q(m_db);
    q.prepare("SELECT user_id FROM users WHERE member_code = :code OR username = :uname LIMIT 1");
    q.bindValue(":code", memberIdOrUsername);
    q.bindValue(":uname", memberIdOrUsername);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getBookIdByIsbn(const QString& isbn) const {
    QSqlQuery q(m_db);
    q.prepare("SELECT book_id FROM books WHERE isbn = :isbn LIMIT 1");
    q.bindValue(":isbn", isbn);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getTransactionIdByCode(const QString& recordCode) const {
    QSqlQuery q(m_db);
    q.prepare("SELECT transaction_id FROM borrow_transactions WHERE record_code = :code LIMIT 1");
    q.bindValue(":code", recordCode);
    if (q.exec() && q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

bool DatabaseManager::loadAllData(QVector<User>& users,
                                  QVector<Book>& books,
                                  QVector<BorrowRecord>& borrows,
                                  QVector<ReservationRecord>& reservations,
                                  QString* errorMessage) {
    if (!m_db.isOpen()) {
        if (errorMessage) *errorMessage = "Database connection is not open.";
        return false;
    }

    users.clear();
    books.clear();
    borrows.clear();
    reservations.clear();

    QSqlQuery qUsers(m_db);
    if (!qUsers.exec("SELECT username, password_hash, role, member_code FROM users WHERE account_status <> 'Inactive' ORDER BY user_id")) {
        if (errorMessage) *errorMessage = qUsers.lastError().text();
        return false;
    }
    while (qUsers.next()) {
        users.append({
            qUsers.value(0).toString(),
            qUsers.value(1).toString(),
            roleFromDbString(qUsers.value(2).toString()),
            qUsers.value(3).toString()
        });
    }

    QSqlQuery qBooks(m_db);
    if (!qBooks.exec("SELECT isbn, title, author, total_copies, available_copies FROM books ORDER BY book_id")) {
        if (errorMessage) *errorMessage = qBooks.lastError().text();
        return false;
    }
    while (qBooks.next()) {
        books.append({
            qBooks.value(0).toString(),
            qBooks.value(1).toString(),
            qBooks.value(2).toString(),
            qBooks.value(3).toInt(),
            qBooks.value(4).toInt()
        });
    }

    QSqlQuery qBorrows(m_db);
    if (!qBorrows.exec(
            "SELECT bt.record_code, u.member_code, b.isbn, b.title, "
            "       bt.borrow_date, bt.due_date, bt.return_date, bt.status, "
            "       COALESCE(f.amount, 0.0), COALESCE(f.amount_paid, 0.0) "
            "FROM borrow_transactions bt "
            "JOIN users u ON bt.user_id = u.user_id "
            "JOIN books b ON bt.book_id = b.book_id "
            "LEFT JOIN fines f ON bt.transaction_id = f.transaction_id "
            "ORDER BY bt.transaction_id")) {
        if (errorMessage) *errorMessage = qBorrows.lastError().text();
        return false;
    }
    while (qBorrows.next()) {
        BorrowRecord rec;
        rec.recordId = qBorrows.value(0).toString();
        rec.memberId = qBorrows.value(1).toString();
        rec.isbn = qBorrows.value(2).toString();
        rec.bookTitle = qBorrows.value(3).toString();
        rec.borrowDate = qBorrows.value(4).toDate();
        rec.dueDate = qBorrows.value(5).toDate();
        rec.returnDate = qBorrows.value(6).toDate();
        QString st = qBorrows.value(7).toString();
        rec.isReturned = (st.compare("Returned", Qt::CaseInsensitive) == 0);
        rec.fineAmount = qBorrows.value(8).toDouble();
        rec.finePaid = qBorrows.value(9).toDouble();
        borrows.append(rec);
    }

    QSqlQuery qRes(m_db);
    if (!qRes.exec(
            "SELECT r.reserve_code, u.member_code, b.isbn, b.title, r.reservation_date, COALESCE(r.notes, '') "
            "FROM reservations r "
            "JOIN users u ON r.user_id = u.user_id "
            "JOIN books b ON r.book_id = b.book_id "
            "WHERE r.status = 'Pending' "
            "ORDER BY r.reservation_date ASC, r.reservation_id ASC")) {
        if (errorMessage) *errorMessage = qRes.lastError().text();
        return false;
    }
    while (qRes.next()) {
        reservations.append({
            qRes.value(0).toString(),
            qRes.value(1).toString(),
            qRes.value(2).toString(),
            qRes.value(3).toString(),
            qRes.value(4).toDate(),
            qRes.value(5).toString()
        });
    }

    return true;
}

bool DatabaseManager::insertUser(const User& user, QString* errorMessage) {
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (username, password_hash, email, role, account_status, member_code) "
              "VALUES (:uname, :pwd, :email, :role, 'Active', :mcode)");
    q.bindValue(":uname", user.username);
    q.bindValue(":pwd", user.password);
    q.bindValue(":email", user.username + "@library.local");
    q.bindValue(":role", roleToDbString(user.role));
    q.bindValue(":mcode", user.memberId);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertBorrow(const BorrowRecord& record, int availableAfterBorrow, QString* errorMessage) {
    int uid = getUserIdByMemberOrName(record.memberId);
    int bid = getBookIdByIsbn(record.isbn);
    if (uid <= 0 || bid <= 0) {
        if (errorMessage) *errorMessage = "Member or Book record not found in MySQL.";
        return false;
    }

    m_db.transaction();
    QSqlQuery qIns(m_db);
    qIns.prepare("INSERT INTO borrow_transactions (record_code, user_id, book_id, borrow_date, due_date, status) "
                 "VALUES (:rcode, :uid, :bid, :bdate, :ddate, 'Borrowed')");
    qIns.bindValue(":rcode", record.recordId);
    qIns.bindValue(":uid", uid);
    qIns.bindValue(":bid", bid);
    qIns.bindValue(":bdate", record.borrowDate);
    qIns.bindValue(":ddate", record.dueDate);
    if (!qIns.exec()) {
        if (errorMessage) *errorMessage = qIns.lastError().text();
        m_db.rollback();
        return false;
    }

    QSqlQuery qUpd(m_db);
    qUpd.prepare("UPDATE books SET available_copies = :avail WHERE book_id = :bid");
    qUpd.bindValue(":avail", availableAfterBorrow);
    qUpd.bindValue(":bid", bid);
    if (!qUpd.exec()) {
        if (errorMessage) *errorMessage = qUpd.lastError().text();
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool DatabaseManager::insertReservation(const ReservationRecord& res, QString* errorMessage) {
    int uid = getUserIdByMemberOrName(res.memberId);
    int bid = getBookIdByIsbn(res.isbn);
    if (uid <= 0 || bid <= 0) {
        if (errorMessage) *errorMessage = "Member or Book record not found in MySQL.";
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO reservations (reserve_code, user_id, book_id, reservation_date, status, notes) "
              "VALUES (:rcode, :uid, :bid, :rdate, 'Pending', :notes)");
    q.bindValue(":rcode", res.reserveId);
    q.bindValue(":uid", uid);
    q.bindValue(":bid", bid);
    q.bindValue(":rdate", res.reserveDate);
    q.bindValue(":notes", res.notes);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::recordBookReturn(const BorrowRecord& record,
                                     int availableAfterReturn,
                                     const QString* notifiedReserveCode,
                                     const QString* notifiedMemberId,
                                     const QString& bookTitle,
                                     QString* errorMessage) {
    int txId = getTransactionIdByCode(record.recordId);
    int bid = getBookIdByIsbn(record.isbn);
    if (txId <= 0 || bid <= 0) {
        if (errorMessage) *errorMessage = "Borrow transaction or book not found in MySQL.";
        return false;
    }

    m_db.transaction();
    QSqlQuery qTx(m_db);
    qTx.prepare("UPDATE borrow_transactions SET return_date = :rdate, status = 'Returned' WHERE transaction_id = :txid");
    qTx.bindValue(":rdate", record.returnDate.isValid() ? record.returnDate : QDate::currentDate());
    qTx.bindValue(":txid", txId);
    if (!qTx.exec()) {
        if (errorMessage) *errorMessage = qTx.lastError().text();
        m_db.rollback();
        return false;
    }

    QSqlQuery qBook(m_db);
    qBook.prepare("UPDATE books SET available_copies = :avail WHERE book_id = :bid");
    qBook.bindValue(":avail", availableAfterReturn);
    qBook.bindValue(":bid", bid);
    if (!qBook.exec()) {
        if (errorMessage) *errorMessage = qBook.lastError().text();
        m_db.rollback();
        return false;
    }

    if (record.fineAmount > 0.0) {
        QSqlQuery qFine(m_db);
        qFine.prepare("INSERT INTO fines (transaction_id, amount, amount_paid, status) "
                      "VALUES (:txid, :amt, :paid, :st) "
                      "ON DUPLICATE KEY UPDATE amount = VALUES(amount), amount_paid = VALUES(amount_paid), status = VALUES(status)");
        qFine.bindValue(":txid", txId);
        qFine.bindValue(":amt", record.fineAmount);
        qFine.bindValue(":paid", record.finePaid);
        qFine.bindValue(":st", record.finePaid >= record.fineAmount ? "Paid" : "Unpaid");
        if (!qFine.exec()) {
            if (errorMessage) *errorMessage = qFine.lastError().text();
            m_db.rollback();
            return false;
        }
    }

    if (notifiedReserveCode && !notifiedReserveCode->isEmpty() && notifiedMemberId && !notifiedMemberId->isEmpty()) {
        QSqlQuery qRes(m_db);
        qRes.prepare("UPDATE reservations SET status = 'Notified' WHERE reserve_code = :rcode");
        qRes.bindValue(":rcode", *notifiedReserveCode);
        qRes.exec();

        int nUid = getUserIdByMemberOrName(*notifiedMemberId);
        if (nUid > 0) {
            QSqlQuery qNotif(m_db);
            qNotif.prepare("INSERT INTO notifications (user_id, message, type, status) "
                           "VALUES (:uid, :msg, 'Reservation', 'Unread')");
            qNotif.bindValue(":uid", nUid);
            qNotif.bindValue(":msg", QString("Reserved book '%1' is now available for pickup.").arg(bookTitle));
            qNotif.exec();
        }
    }

    return m_db.commit();
}

bool DatabaseManager::markFinePaid(const QString& recordCode, double paidAmount, QString* errorMessage) {
    int txId = getTransactionIdByCode(recordCode);
    if (txId <= 0) {
        if (errorMessage) *errorMessage = "Borrow transaction not found in MySQL.";
        return false;
    }

    QSqlQuery q(m_db);
    q.prepare("INSERT INTO fines (transaction_id, amount, amount_paid, status) "
              "VALUES (:txid, :amt, :paid, 'Paid') "
              "ON DUPLICATE KEY UPDATE amount_paid = :paid2, status = 'Paid'");
    q.bindValue(":txid", txId);
    q.bindValue(":amt", paidAmount);
    q.bindValue(":paid", paidAmount);
    q.bindValue(":paid2", paidAmount);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::cancelReservation(const QString& reserveCode, QString* errorMessage) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE reservations SET status = 'Cancelled' WHERE reserve_code = :rcode");
    q.bindValue(":rcode", reserveCode);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateUserRole(const QString& username, UserRole newRole, QString* errorMessage) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET role = :role WHERE username = :uname");
    q.bindValue(":role", roleToDbString(newRole));
    q.bindValue(":uname", username);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeUser(const QString& username, QString* errorMessage) {
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET account_status = 'Inactive' WHERE username = :uname");
    q.bindValue(":uname", username);
    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }
    return true;
}