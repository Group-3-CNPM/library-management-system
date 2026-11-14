#include <iostream>
#include <string>
#include <vector>

using namespace std;
class Fine {
private:
    int fineID;
    int transactionID;
    double amount;
    string status; // Unpaid, Paid

public:
    double calculateFine(int overdueDays, double dailyRate) {
        // TODO: Tính tiền phạt = số ngày quá hạn * mức phí ngày (FR7, UC-08)
        return overdueDays * dailyRate;
    }

    bool recordPayment(double amountPaid) {
        // TODO: Ghi nhận thanh toán và cập nhật trạng thái thành "Paid" (FR7)
        return true;
    }

    vector<Fine> getOutstandingFines(int uID) {
        // TODO: Lấy danh sách các khoản phạt chưa thanh toán của user
        vector<Fine> fines;
        return fines;
    }

    void updateStatus(string newStatus) {
        // TODO: Cập nhật trạng thái Fine
    }

    Fine getFineDetails() {
        // TODO: Trả về chi tiết phiếu phạt
        return *this;
    }
};
