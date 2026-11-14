#include <iostream>
#include <string>
#include <vector>

using namespace std;
class Notification {
private:
    int notificationID;
    int userID;
    string message;
    string sentDate;
    string type;   // Reminder, Overdue, Reservation, System
    string status; // Read, Unread

public:
    bool sendNotification() {
        // TODO: Gửi thông báo tự động cho người dùng (FR9)
        return true;
    }

    void markAsRead() {
        // TODO: Đánh dấu thông báo đã đọc
    }

    vector<Notification> getNotifications(int uID) {
        // TODO: Lấy danh sách thông báo của người dùng
        vector<Notification> list;
        return list;
    }
};
