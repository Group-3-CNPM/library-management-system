#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
class BorrowTransaction {
private:
    int transactionID;
    int userID;
    int bookID;
    string borrowDate;
    string dueDate;
    string returnDate;
    string status; // Trạng thái: "Borrowed", "Returned", "Overdue"

public:
    BorrowTransaction() {
        this->transactionID = 0;
        this->userID = 0;
        this->bookID = 0;
        this->borrowDate = "";
        this->dueDate = "";
        this->returnDate = "";
        this->status = "Borrowed";
    }

    BorrowTransaction(int tID, int uID, int bID, string bDate, string dDate) {
        this->transactionID = tID;
        this->userID = uID;
        this->bookID = bID;
        this->borrowDate = bDate;
        this->dueDate = dDate;
        this->returnDate = "";
        this->status = "Borrowed";
    }

    int getTransactionID() const { 
        return transactionID; 
    }

    int getUserID() const { 
        return userID; 
    }

    int getBookID() const { 
        return bookID; 
    }

    string getBorrowDate() const { 
        return borrowDate; 
    }

    string getDueDate() const { 
        return dueDate; 
    }

    string getReturnDate() const { 
        return returnDate; 
    }

    string getStatus() const { 
        return status; 
    }

    void setReturnDate(string rDate) { 
        this->returnDate = rDate; 
    }

    void setStatus(string s) { 
        this->status = s; 
    }

    BorrowTransaction createTransaction(int uID, int bID, string dDate) {
        // Tạo bản ghi mượn sách mới, giảm số lượng sách trong kho (FR4)
        if (uID <= 0 || bID <= 0) {
            cout << "Lỗi: User ID hoặc Book ID không hợp lệ!" << endl;
            return BorrowTransaction();
        }
        //Viết câu lệnh SQL tạo giao dịch mượn mới và trừ số lượng sách đã mượn vào số lượng trong data base
        cout << "Đã tạo thành công giao dịch mượn sách cho User ID: " << uID << " (Book ID: " << bID << ")" << endl;
        return *this;
    }

    bool processReturn(string rDate) {
        // Xử lý trả sách, cập nhật ngày trả thực tế và tăng kho (FR5)
        if (rDate.empty()) {
            cout << "Lỗi: Ngày trả sách không được để trống!" << endl;
            return false;
        }
        this->returnDate = rDate;
        this->status = "Returned";
        //Viết câu lệnh SQL:
        // Cập nhập ngày trả sách thực tế returndate
        // Cập nhập lại số lượng sách trong data base sau khi trả
        // Gọi class Fine để tính tiền phạt nếu trả sách quá hạn
        cout << "Đã xử lý trả sách thành công cho Transaction ID: " << this->transactionID << endl;
        return true;
    }

    int calculateOverdueDays() {
        // Tính số ngày quá hạn so với DueDate (FR5, UC-08)
        // Viết thêm lệnh xử lí bằng SQL
        // Xử lý logic tính toán khoảng cách ngày (ví dụ: so sánh actual returnDate với dueDate)
        // Nếu trả trước hạn hoặc đúng hạn, trả về 0. Nếu trễ, trả về số ngày chênh lệch.
        int overdueDays = 0; 
        cout << "Đang tính toán số ngày quá hạn..." << endl;
        return overdueDays;
    }

    bool isOverdue() {
        // Kiểm tra xem giao dịch có bị quá hạn hay không
        if (this->status == "Overdue" || (this->returnDate.empty() && calculateOverdueDays() > 0)) {
            return true;
        }
        return false;
    }

    vector<BorrowTransaction> getBorrowHistory(int uID) {
        // Lấy lịch sử mượn sách của một thành viên (FR8, UC-09)
        vector<BorrowTransaction> historyList;
        if (uID <= 0) {
            cout << "Lỗi: User ID không hợp lệ!" << endl;
            return historyList;
        }
        // Viết câu lệnh SQL hiẹn ra lịch sử mượn sách của user theo ID
        cout << "Đang truy vấn lịch sử mượn sách cho User ID: " << uID << endl;
        return historyList;
    }

    BorrowTransaction getTransactionSummary() {
        // Trả về tóm tắt thông tin giao dịch
        cout << "=== TÓM TẮT GIAO DỊCH MƯỢN SÁCH ===" << endl;
        cout << "Transaction ID: " << this->transactionID << endl;
        cout << "User ID: " << this->userID << endl;
        cout << "Book ID: " << this->bookID << endl;
        cout << "Ngày mượn: " << this->borrowDate << endl;
        cout << "Ngày đến hạn: " << this->dueDate << endl;
        cout << "Ngày trả: " << (this->returnDate.empty() ? "Chưa trả" : this->returnDate) << endl;
        cout << "Trạng thái: " << this->status << endl;
        return *this;
    }
};
