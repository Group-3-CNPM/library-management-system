#include <iostream>
#include <string>
#include <vector>

using namespace std;
class BorrowTransaction {
private:
    int transactionID;
    int userID;
    int bookID;
    string borrowDate;
    string dueDate;
    string returnDate;

public:
    BorrowTransaction createTransaction(int uID, int bID, string dDate) {
        // TODO: Tạo bản ghi mượn sách mới, giảm số lượng sách trong kho (FR4)
        return BorrowTransaction();
    }

    bool processReturn(string rDate) {
        // TODO: Xử lý trả sách, cập nhật ngày trả thực tế và tăng kho (FR5)
        return true;
    }

    int calculateOverdueDays() {
        // TODO: Tính số ngày quá hạn so với DueDate (FR5, UC-08)
        return 0;
    }

    bool isOverdue() {
        // TODO: Kiểm tra xem giao dịch có bị quá hạn hay không
        return false;
    }

    vector<BorrowTransaction> getBorrowHistory(int uID) {
        // TODO: Lấy lịch sử mượn sách của một thành viên (FR8, UC-09)
        vector<BorrowTransaction> history;
        return history;
    }

    BorrowTransaction getTransactionSummary() {
        // TODO: Trả về tóm tắt thông tin giao dịch
        return *this;
    }
};
