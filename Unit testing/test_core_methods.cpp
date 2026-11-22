#include <gtest/gtest.h>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr long long kFineRatePerDay = 5000LL;
constexpr int kDefaultLoanDays = 14;

class User {
public:
    enum class Role {
        Member,
        Librarian,
        Administrator
    };

    User(std::string id, std::string username, std::string password, Role role, bool active = true)
        : id_(std::move(id)),
          username_(std::move(username)),
          password_(std::move(password)),
          role_(role),
          active_(active) {}

    const std::string& id() const { return id_; }
    const std::string& username() const { return username_; }
    Role role() const { return role_; }
    bool isActive() const { return active_; }

    bool authenticate(const std::string& inputPassword) const {
        if (!active_) {
            return false;
        }
        return password_ == inputPassword;
    }

    bool canManageCatalog() const {
        return active_ && (role_ == Role::Librarian || role_ == Role::Administrator);
    }

    bool canProcessReturn() const {
        return active_ && (role_ == Role::Librarian || role_ == Role::Administrator);
    }

    bool canManageUsers() const {
        return active_ && role_ == Role::Administrator;
    }

    static bool isValidRegistration(const std::string& username,
                                    const std::string& password,
                                    const std::vector<std::string>& existingUsernames) {
        if (username.empty() || password.empty()) {
            return false;
        }
        for (const auto& existing : existingUsernames) {
            if (existing == username) {
                return false;
            }
        }
        return true;
    }

private:
    std::string id_;
    std::string username_;
    std::string password_;
    Role role_;
    bool active_;
};

class Book {
public:
    Book(std::string isbn, std::string title, std::string author, int totalCopies, int availableCopies)
        : isbn_(std::move(isbn)),
          title_(std::move(title)),
          author_(std::move(author)),
          totalCopies_(totalCopies),
          availableCopies_(availableCopies) {
        if (totalCopies_ < 0 || availableCopies_ < 0 || availableCopies_ > totalCopies_) {
            throw std::invalid_argument("Invalid book copy counts");
        }
    }

    const std::string& isbn() const { return isbn_; }
    const std::string& title() const { return title_; }
    const std::string& author() const { return author_; }
    int totalCopies() const { return totalCopies_; }
    int availableCopies() const { return availableCopies_; }

    bool isAvailable() const {
        return availableCopies_ > 0;
    }

    bool borrowCopy() {
        if (!isAvailable()) {
            return false;
        }
        --availableCopies_;
        return true;
    }

    bool returnCopy() {
        if (availableCopies_ >= totalCopies_) {
            return false;
        }
        ++availableCopies_;
        return true;
    }

    bool matchesKeyword(const std::string& rawKeyword) const {
        std::string query = trimAndLower(rawKeyword);
        if (query.empty()) {
            return true;
        }
        return toLower(title_).find(query) != std::string::npos ||
               toLower(author_).find(query) != std::string::npos ||
               toLower(isbn_).find(query) != std::string::npos;
    }

private:
    std::string isbn_;
    std::string title_;
    std::string author_;
    int totalCopies_;
    int availableCopies_;

    static std::string toLower(const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return out;
    }

    static std::string trimAndLower(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
            ++start;
        }
        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
            --end;
        }
        return toLower(s.substr(start, end - start));
    }
};

class Fine {
public:
    static int calculateOverdueDays(int borrowDayOffset, int dueDayOffset, int returnDayOffset) {
        if (dueDayOffset < borrowDayOffset) {
            throw std::invalid_argument("Due date cannot be earlier than borrow date");
        }
        int overdue = returnDayOffset - dueDayOffset;
        return overdue > 0 ? overdue : 0;
    }

    static long long calculateFineAmount(int overdueDays, long long ratePerDay = kFineRatePerDay) {
        if (overdueDays <= 0 || ratePerDay <= 0) {
            return 0LL;
        }
        return static_cast<long long>(overdueDays) * ratePerDay;
    }

    static std::string formatVnd(long long amount) {
        if (amount <= 0) {
            return "0 VND";
        }
        std::string digits = std::to_string(amount);
        std::string formatted;
        int count = 0;
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            if (count > 0 && count % 3 == 0) {
                formatted.push_back('.');
            }
            formatted.push_back(*it);
            ++count;
        }
        std::reverse(formatted.begin(), formatted.end());
        return formatted + " VND";
    }
};

class BorrowTransaction {
public:
    enum class Status {
        Borrowed,
        Returned
    };

    BorrowTransaction(std::string transactionId,
                      std::string memberId,
                      std::string isbn,
                      int borrowDayOffset,
                      int dueDayOffset)
        : transactionId_(std::move(transactionId)),
          memberId_(std::move(memberId)),
          isbn_(std::move(isbn)),
          borrowDayOffset_(borrowDayOffset),
          dueDayOffset_(dueDayOffset),
          status_(Status::Borrowed),
          fineAmount_(0LL),
          finePaid_(false) {
        if (transactionId_.empty() || memberId_.empty() || isbn_.empty()) {
            throw std::invalid_argument("Missing required borrow fields");
        }
        if (dueDayOffset_ < borrowDayOffset_) {
            throw std::invalid_argument("Due date cannot be earlier than borrow date");
        }
    }

    const std::string& transactionId() const { return transactionId_; }
    const std::string& memberId() const { return memberId_; }
    const std::string& isbn() const { return isbn_; }
    int dueDayOffset() const { return dueDayOffset_; }
    Status status() const { return status_; }
    long long fineAmount() const { return fineAmount_; }
    bool isFinePaid() const { return finePaid_; }

    bool markReturned(int returnDayOffset, Book& book) {
        if (status_ == Status::Returned) {
            return false;
        }
        int overdueDays = Fine::calculateOverdueDays(borrowDayOffset_, dueDayOffset_, returnDayOffset);
        fineAmount_ = Fine::calculateFineAmount(overdueDays);
        finePaid_ = (fineAmount_ == 0LL);
        status_ = Status::Returned;
        return book.returnCopy();
    }

    bool markFinePaid() {
        if (status_ != Status::Returned || fineAmount_ <= 0 || finePaid_) {
            return false;
        }
        finePaid_ = true;
        return true;
    }

private:
    std::string transactionId_;
    std::string memberId_;
    std::string isbn_;
    int borrowDayOffset_;
    int dueDayOffset_;
    Status status_;
    long long fineAmount_;
    bool finePaid_;
};

class Reservation {
public:
    enum class Status {
        Pending,
        Cancelled,
        Fulfilled
    };

    Reservation(std::string reservationId, std::string memberId, std::string isbn, std::string notes)
        : reservationId_(std::move(reservationId)),
          memberId_(std::move(memberId)),
          isbn_(std::move(isbn)),
          notes_(std::move(notes)),
          status_(Status::Pending) {}

    const std::string& reservationId() const { return reservationId_; }
    const std::string& memberId() const { return memberId_; }
    const std::string& isbn() const { return isbn_; }
    Status status() const { return status_; }

    static bool canCreateReservation(const Book& book,
                                     const std::string& memberId,
                                     const std::vector<Reservation>& existingReservations) {
        if (memberId.empty() || book.isAvailable()) {
            return false;
        }
        for (const auto& res : existingReservations) {
            if (res.memberId() == memberId &&
                res.isbn() == book.isbn() &&
                res.status() == Status::Pending) {
                return false;
            }
        }
        return true;
    }

    bool cancel() {
        if (status_ != Status::Pending) {
            return false;
        }
        status_ = Status::Cancelled;
        return true;
    }

private:
    std::string reservationId_;
    std::string memberId_;
    std::string isbn_;
    std::string notes_;
    Status status_;
};

class Notification {
public:
    static std::string buildReservedAvailableMessage(const std::string& memberId, const std::string& bookTitle) {
        return "Notification sent to " + memberId + ": Reserved book '" + bookTitle + "' is now available for pickup.";
    }

    static std::string buildOverdueNotice(const std::string& memberId,
                                          const std::string& bookTitle,
                                          int overdueDays) {
        long long fine = Fine::calculateFineAmount(overdueDays);
        return "Overdue notice for " + memberId + " (" + bookTitle + "): " +
               std::to_string(overdueDays) + " overdue days, fine: " + Fine::formatVnd(fine);
    }
};

} // namespace

// 1. User tests
TEST(UserTest, AuthenticationAndRolePermissions) {
    User member("MEM-101", "member", "123", User::Role::Member, true);
    User librarian("LIB-001", "librarian", "123", User::Role::Librarian, true);
    User admin("ADM-001", "admin", "123", User::Role::Administrator, true);

    EXPECT_TRUE(member.authenticate("123"));
    EXPECT_FALSE(member.authenticate("wrongpass"));
    EXPECT_FALSE(member.canManageCatalog());
    EXPECT_FALSE(member.canProcessReturn());
    EXPECT_FALSE(member.canManageUsers());

    EXPECT_TRUE(librarian.canManageCatalog());
    EXPECT_TRUE(librarian.canProcessReturn());
    EXPECT_FALSE(librarian.canManageUsers());

    EXPECT_TRUE(admin.canManageCatalog());
    EXPECT_TRUE(admin.canProcessReturn());
    EXPECT_TRUE(admin.canManageUsers());
}

TEST(UserTest, RegistrationValidationRejectsEmptyOrDuplicateUsername) {
    std::vector<std::string> existing = {"member", "librarian", "admin"};

    EXPECT_FALSE(User::isValidRegistration("", "abc123", existing));
    EXPECT_FALSE(User::isValidRegistration("tester01", "", existing));
    EXPECT_FALSE(User::isValidRegistration("member", "abc123", existing));
    EXPECT_TRUE(User::isValidRegistration("newmember", "abc123", existing));
}

// 2. Book tests
TEST(BookTest, BorrowAndReturnUpdatesAvailableCopies) {
    Book book("978-0201633610", "Design Patterns", "Erich Gamma et al.", 5, 4);

    EXPECT_TRUE(book.isAvailable());
    EXPECT_TRUE(book.borrowCopy());
    EXPECT_EQ(book.availableCopies(), 3);

    EXPECT_TRUE(book.returnCopy());
    EXPECT_EQ(book.availableCopies(), 4);
}

TEST(BookTest, OutOfStockBookCannotBeBorrowedAndSearchMatchesCaseInsensitive) {
    Book cleanCode("978-0132350884", "Clean Code", "Robert C. Martin", 3, 0);

    EXPECT_FALSE(cleanCode.isAvailable());
    EXPECT_FALSE(cleanCode.borrowCopy());
    EXPECT_EQ(cleanCode.availableCopies(), 0);

    EXPECT_TRUE(cleanCode.matchesKeyword("clean"));
    EXPECT_TRUE(cleanCode.matchesKeyword("978-0132350884"));
    EXPECT_TRUE(cleanCode.matchesKeyword("  Martin  "));
    EXPECT_FALSE(cleanCode.matchesKeyword("xyz123"));
}

// 3. Fine tests
TEST(FineTest, OverdueDaysAndFineCalculationBoundaries) {
    EXPECT_EQ(Fine::calculateOverdueDays(-18, -4, -6), 0);
    EXPECT_EQ(Fine::calculateOverdueDays(-18, -4, -4), 0);
    EXPECT_EQ(Fine::calculateFineAmount(0), 0LL);

    EXPECT_EQ(Fine::calculateFineAmount(Fine::calculateOverdueDays(-18, -4, -3)), 5000LL);
    EXPECT_EQ(Fine::calculateFineAmount(Fine::calculateOverdueDays(-18, -4, 0)), 20000LL);
    EXPECT_EQ(Fine::calculateFineAmount(Fine::calculateOverdueDays(-18, -4, 6)), 50000LL);

    EXPECT_EQ(Fine::formatVnd(20000LL), "20.000 VND");
    EXPECT_EQ(Fine::formatVnd(50000LL), "50.000 VND");
    EXPECT_THROW(Fine::calculateOverdueDays(0, -1, 0), std::invalid_argument);
}

// 4. BorrowTransaction tests
TEST(BorrowTransactionTest, ReturnOverdueBookCalculatesFineAndMarksFinePaid) {
    Book cleanCode("978-0132350884", "Clean Code", "Robert C. Martin", 3, 0);
    BorrowTransaction tx("BRW-001", "MEM-101", "978-0132350884", -18, -4);

    EXPECT_EQ(tx.dueDayOffset() - (-18), kDefaultLoanDays);
    EXPECT_TRUE(tx.markReturned(0, cleanCode));
    EXPECT_EQ(tx.status(), BorrowTransaction::Status::Returned);
    EXPECT_EQ(tx.fineAmount(), 20000LL);
    EXPECT_FALSE(tx.isFinePaid());
    EXPECT_EQ(cleanCode.availableCopies(), 1);

    EXPECT_FALSE(tx.markReturned(0, cleanCode));
    EXPECT_EQ(cleanCode.availableCopies(), 1);

    EXPECT_TRUE(tx.markFinePaid());
    EXPECT_TRUE(tx.isFinePaid());
    EXPECT_FALSE(tx.markFinePaid());
}

// 5. Reservation tests
TEST(ReservationTest, QueueEligibilityAndCancellation) {
    Book cleanCode("978-0132350884", "Clean Code", "Robert C. Martin", 3, 0);
    Book designPatterns("978-0201633610", "Design Patterns", "Erich Gamma et al.", 5, 4);

    std::vector<Reservation> queue;
    EXPECT_FALSE(Reservation::canCreateReservation(designPatterns, "MEM-101", queue));
    EXPECT_FALSE(Reservation::canCreateReservation(cleanCode, "", queue));
    EXPECT_TRUE(Reservation::canCreateReservation(cleanCode, "MEM-105", queue));

    queue.emplace_back("RES-001", "MEM-105", "978-0132350884", "Capstone");
    EXPECT_FALSE(Reservation::canCreateReservation(cleanCode, "MEM-105", queue));
    EXPECT_TRUE(Reservation::canCreateReservation(cleanCode, "MEM-101", queue));

    EXPECT_TRUE(queue[0].cancel());
    EXPECT_EQ(queue[0].status(), Reservation::Status::Cancelled);
    EXPECT_FALSE(queue[0].cancel());
}

// 6. Notification tests
TEST(NotificationTest, BuildsExpectedPickupAndOverdueMessages) {
    std::string pickupMsg = Notification::buildReservedAvailableMessage("MEM-105", "Clean Code");
    EXPECT_NE(pickupMsg.find("MEM-105"), std::string::npos);
    EXPECT_NE(pickupMsg.find("Clean Code"), std::string::npos);

    std::string overdueMsg = Notification::buildOverdueNotice("MEM-101", "Clean Code", 4);
    EXPECT_NE(overdueMsg.find("4 overdue days"), std::string::npos);
    EXPECT_NE(overdueMsg.find("20.000 VND"), std::string::npos);
}