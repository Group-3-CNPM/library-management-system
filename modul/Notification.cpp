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

    static inline vector<Notification> notifications;
    static inline int nextNotificationID = 1;

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

    static bool isValidType(const string& t) {
        return t == "Reminder" || t == "Overdue" || t == "Reservation" || t == "System";
    }

    static bool isValidStatus(const string& s) {
        return s == "Read" || s == "Unread";
    }
public:
    Notification()
        : notificationID(0), userID(0), message(""), sentDate(""),
          type("System"), status("Unread") {}

    Notification(int nID, int uID, const string& msg, const string& date,
                 const string& nType = "System", const string& nStatus = "Unread")
        : notificationID(nID), userID(uID), message(msg), sentDate(date),
          type(isValidType(nType) ? nType : "System"),
          status(isValidStatus(nStatus) ? nStatus : "Unread") {}

    int getNotificationID() const { return notificationID; }
    int getUserID() const { return userID; }
    string getMessage() const { return message; }
    string getSentDate() const { return sentDate; }
    string getType() const { return type; }
    string getStatus() const { return status; }

    // Setters cho dữ liệu đầu vào
    void setRecipient(int uID, const string& msg, const string& nType = "System") {
        userID = uID;
        message = msg;
        if (isValidType(nType)) {
            type = nType;
        }
        status = "Unread";
    }

    bool sendNotification() {
        // Gửi thông báo tự động cho người dùng (FR9)
        if (userID <= 0 || message.empty()) {
            return false;
        }

        if (sentDate.empty()) {
            sentDate = getCurrentDateString();
        }
        if (!isValidType(type)) {
            type = "System";
        }
        status = "Unread";

        if (notificationID == 0) {
            notificationID = nextNotificationID++;
            notifications.push_back(*this);
        } else {
            bool found = false;
            for (auto& n : notifications) {
                if (n.notificationID == notificationID) {
                    n = *this;
                    found = true;
                    break;
                }
            }
            if (!found) {
                notifications.push_back(*this);
            }
        }

        cout << "[Notification #" << notificationID << " | " << type << " | " << sentDate
             << " -> User #" << userID << "] " << message << endl;
        return true;
    }
    // Hàm hỗ trợ tạo và gửi ngay một thông báo
    static bool dispatch(int uID, const string& msg, const string& nType = "System") {
        Notification n(0, uID, msg, getCurrentDateString(), nType, "Unread");
        return n.sendNotification();
    }

    void markAsRead() {
        // Đánh dấu thông báo đã đọc
        status = "Read";
        if (notificationID > 0) {
            for (auto& n : notifications) {
                if (n.notificationID == notificationID) {
                    n.status = "Read";
                    break;
                }
            }
        }
    }

    vector<Notification> getNotifications(int uID) {
        // Lấy danh sách thông báo của người dùng
        vector<Notification> list;
        for (const auto& n : notifications) {
            if (n.userID == uID) {
                list.push_back(n);
            }
        }
        return list;
    }
};
