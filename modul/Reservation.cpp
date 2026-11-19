#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
class Reservation {
private:
    int reservationID;
    int userID;
    int bookID;
    string reservationDate;
    string status; // Pending, Fulfilled, Cancelled

    // Danh sách lưu tạm trong bộ nhớ (sẽ đồng bộ với Database khi ghép tầng DB)
    static inline vector<Reservation> reservations;
    static inline int nextReservationID = 1;

    static string getCurrentDateString() {
        time_t now = time(nullptr);
        tm localTm{};
    #ifdef _WIN32
            localtime_s(&localTm, &now);
    #else
            localtime_r(&now, &localTm);
    #endif
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", &localTm);
        return string(buf);
    }

    static bool isValidStatus(const string& s) {
        return s == "Pending" || s == "Fulfilled" || s == "Cancelled";
    }
public:
    Reservation()
        : reservationID(0), userID(0), bookID(0), reservationDate(""), status("Pending") {}

    Reservation(int resID, int uID, int bID, const string& resDate, const string& st = "Pending")
        : reservationID(resID), userID(uID), bookID(bID), reservationDate(resDate),
          status(isValidStatus(st) ? st : "Pending") {}

    int getReservationID() const { return reservationID; }
    int getUserID() const { return userID; }
    int getBookID() const { return bookID; }
    string getReservationDate() const { return reservationDate; }
    string getStatus() const { return status; }

    Reservation createReservation(int uID, int bID) {
        // Tạo yêu cầu đặt trước khi sách hết (FR6)
        if (uID <= 0 || bID <= 0) {
            return Reservation();
        }

        // Tránh tạo trùng yêu cầu Pending cho cùng user + book
        for (const auto& r : reservations) {
            if (r.userID == uID && r.bookID == bID && r.status == "Pending") {
                return r;
            }
        return Reservation();
        }
        Reservation newRes(
            nextReservationID++,
            uID,
            bID,
            getCurrentDateString(),
            "Pending"
        );

        reservations.push_back(newRes);
        *this = newRes;
        return *this;
    }

    bool cancelReservation(int resID) {
        // Thành viên hủy yêu cầu đặt trước (FR6)
        for (auto& r : reservations) {
            if (r.reservationID == resID) {
                if (r.status == "Pending") {
                    r.status = "Cancelled";
                    if (this->reservationID == resID) {
                        this->status = "Cancelled";
                    }
                    return true;
                }
                return false; // Không hủy được nếu đã Fulfilled hoặc Cancelled trước đó
            }
        }
        return false;
    }

    void updateStatus(string newStatus) {
        // Cập nhật trạng thái đặt trước
        if (!isValidStatus(newStatus)) {
            return;
        }
        status = newStatus;
        if (reservationID > 0) {
            for (auto& r : reservations) {
                if (r.reservationID == reservationID) {
                    r.status = newStatus;
                    break;
                }
            }
        }
    }

    void notifyMember(int uID, string msg) {
        // Gửi thông báo khi sách được trả về cho người đặt (FR6, FR9)
        if (uID <= 0 || msg.empty()) {
            return;
        }
        cout << "[Notification -> User #" << uID << "] " << msg << endl;
    }

    vector<Reservation> getReservationQueue(int bID) {
        // Quản lý hàng đợi đặt trước theo nguyên tắc FIFO (FR6)
        vector<Reservation> queue;
        for (const auto& r : reservations) {
            if (r.bookID == bID && r.status == "Pending") {
                queue.push_back(r);
            }
        }
        sort(queue.begin(), queue.end(), [](const Reservation& a, const Reservation& b) {
            if (a.reservationDate != b.reservationDate) {
                return a.reservationDate < b.reservationDate;
            }
            return a.reservationID < b.reservationID;
        });
        return queue;
    }

    Reservation getReservationInfo() {
        // Lấy thông tin chi tiết đặt trước
        return *this;
    }
};
