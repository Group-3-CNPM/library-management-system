#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
class Fine {
private:
    int fineID;
    int transactionID;
    int userID;
    double amount;
    string status; // Unpaid, Paid

    static inline vector<Fine> finesStore;
    static inline int nextFineID = 1;

    static bool isValidStatus(const string& s) {
        return s == "Unpaid" || s == "Paid";
    }

    void syncToStore() {
        if (fineID <= 0) {
            return;
        }
        for (auto& f : finesStore) {
            if (f.fineID == fineID) {
                f = *this;
                return;
            }
        }
        finesStore.push_back(*this);
    }

public:
    Fine()
        : fineID(0), transactionID(0), userID(0), amount(0.0), status("Unpaid") {}

    Fine(int fID, int tID, double amt, const string& st = "Unpaid", int uID = 0)
        : fineID(fID),
          transactionID(tID),
          userID(uID),
          amount(amt > 0.0 ? amt : 0.0),
          status(isValidStatus(st) ? st : "Unpaid") {}

    // Getters
    int getFineID() const { return fineID; }
    int getTransactionID() const { return transactionID; }
    int getUserID() const { return userID; }
    double getAmount() const { return amount; }
    string getStatus() const { return status; }

    // Tạo phiếu phạt mới từ giao dịch quá hạn (FR7, UC-08)
    Fine createFine(int tID, int uID, int overdueDays, double dailyRate) {
        if (tID <= 0 || uID <= 0) {
            return Fine();
        }
        double calculated = calculateFine(overdueDays, dailyRate);
        Fine newFine(
            nextFineID++,
            tID,
            calculated,
            calculated > 0.0 ? "Unpaid" : "Paid",
            uID
        );
        finesStore.push_back(newFine);
        *this = newFine;
        return *this;
    }

    double calculateFine(int overdueDays, double dailyRate) {
        // Tính tiền phạt = số ngày quá hạn * mức phí ngày (FR7, UC-08)
        if (overdueDays <= 0 || dailyRate <= 0.0) {
            return 0.0;
        }
        return static_cast<double>(overdueDays) * dailyRate;
    }

    bool recordPayment(double amountPaid) {
        // Ghi nhận thanh toán và cập nhật trạng thái thành "Paid" (FR7)
        if (amountPaid <= 0.0) {
            return false;
        }
        if (status == "Paid" || amount <= 0.0) {
            status = "Paid";
            amount = 0.0;
            syncToStore();
            return true;
        }

        if (amountPaid >= amount) {
            amount = 0.0;
            status = "Paid";
        } else {
            amount -= amountPaid;
            if (fabs(amount) < 1e-6) {
                amount = 0.0;
                status = "Paid";
            } else {
                status = "Unpaid";
            }
        }

        syncToStore();
        return true;
    }

    vector<Fine> getOutstandingFines(int uID) {
        // Lấy danh sách các khoản phạt chưa thanh toán của user
        vector<Fine> fines;
        for (const auto& f : finesStore) {
        if (f.userID == uID && f.status == "Unpaid" && f.amount > 0.0) {
            fines.push_back(f);
            }
        }
        return fines;
    }

    void updateStatus(string newStatus) {
        // Cập nhật trạng thái Fine
        if (!isValidStatus(newStatus)) {
            return;
        }
        status = newStatus;
        if (status == "Paid") {
            amount = 0.0;
        }
        syncToStore();
    }

    Fine getFineDetails() {
        // Trả về chi tiết phiếu phạt
        return *this;
    }
};
