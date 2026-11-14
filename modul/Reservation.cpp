#include <iostream>
#include <string>
#include <vector>

using namespace std;
class Reservation {
private:
    int reservationID;
    int userID;
    int bookID;
    string reservationDate;
    string status; // Pending, Fulfilled, Cancelled

public:
    Reservation createReservation(int uID, int bID) {
        // TODO: Tạo yêu cầu đặt trước khi sách hết (FR6)
        return Reservation();
    }

    bool cancelReservation(int resID) {
        // TODO: Thành viên hủy yêu cầu đặt trước (FR6)
        return true;
    }

    void updateStatus(string newStatus) {
        // TODO: Cập nhật trạng thái đặt trước
    }

    void notifyMember(int uID, string msg) {
        // TODO: Gửi thông báo khi sách được trả về cho người đặt (FR6, FR9)
    }

    vector<Reservation> getReservationQueue(int bID) {
        // TODO: Quản lý hàng đợi đặt trước theo nguyên tắc FIFO (FR6)
        vector<Reservation> queue;
        return queue;
    }

    Reservation getReservationInfo() {
        // TODO: Lấy thông tin chi tiết đặt trước
        return *this;
    }
};
