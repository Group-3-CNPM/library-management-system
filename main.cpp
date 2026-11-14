#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Quản lí người dùng
class User {
private:
    int userID;
    string userName;
    string email;
    string password;
    string role;          // phân quyềnAdministrator, Librarian, Member
    string accountStatus; // trạng thái tài khoản: Active, Suspended, Inactive

public:
    // Các hàm chức năng chính
    bool login(string name, string pass) {
        // TODO: Viết logic kiểm tra username và password từ DB
        return true; 
    }

    void logout() {
        // TODO: Xử lý hủy phiên đăng nhập hiện tại
    }

    bool updateProfile(string name, string mail) {
        // TODO: Cập nhật thông tin cá nhân của Member/Librarian
        return true;
    }

    bool changePassword(string oldPass, string newPass) {
        // TODO: Xác thực mật khẩu cũ và lưu mật khẩu mới đã mã hóa
        return true;
    }

    bool addUser(User userData) {
        // TODO: Admin/Librarian thêm tài khoản mới (FR1, FR8)
        return true;
    }

    bool updateUser(int id, User userData) {
        // TODO: Cập nhật thông tin tài khoản theo ID
        return true;
    }

    bool deactivateUser(int id) {
        // TODO: Vô hiệu hóa tài khoản vi phạm hoặc có phạt chưa trả (FR8)
        return true;
    }

    User showProfile() {
        // TODO: Trả về thông tin chi tiết của người dùng
        return *this;
    }
};

// ==========================================
// 2. LỚP BOOK (Quản lý danh mục sách và kho)
// ==========================================
class Book {
private:
    int bookID;
    string isbn;
    string title;
    string author;
    string category;
    int quantity; // Số lượng bản sao khả dụng

public:
    bool addBook(Book bookData) {
        // TODO: Thêm đầu sách mới vào CSDL (FR2, FR16)
        return true;
    }

    bool updateBook(int id, Book bookData) {
        // TODO: Cập nhật thông tin sách (tiêu đề, tác giả, v.v.)
        return true;
    }

    bool removeBook(int id) {
        // TODO: Xóa hoặc ẩn sách hỏng/cũ khỏi danh mục (FR2)
        return true;
    }

    vector<Book> searchBook(string keyword) {
        // TODO: Tìm kiếm sách theo tiêu đề, tác giả, ISBN hoặc thể loại (FR3)
        vector<Book> results;
        return results;
    }

    bool checkAvailability() {
        // TODO: Kiểm tra số lượng quantity > 0 hay không (UC-05)
        return (quantity > 0);
    }

    void updateQuantity(int changeAmount) {
        // TODO: Tăng/giảm số lượng tồn kho khi mượn hoặc trả sách (FR4, FR5)
        quantity += changeAmount;
    }

    Book getBookDetails() {
        // TODO: Trả về thông tin chi tiết của sách (UC-03)
        return *this;
    }
};

// ==========================================
// 3. LỚP BORROWTRANSACTION (Quản lý mượn/trả sách)
// ==========================================
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

// ==========================================
// 4. LỚP RESERVATION (Quản lý đặt trước sách)
// ==========================================
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

// ==========================================
// 5. LỚP FINE (Quản lý tiền phạt quá hạn)
// ==========================================
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

// ==========================================
// 6. LỚP NOTIFICATION (Quản lý hệ thống thông báo)
// ==========================================
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

// ==========================================
// HÀM MAIN CHÍNH ĐỂ KIỂM THỬ KHUNG CHƯƠNG TRÌNH
// ==========================================
int main() {
    cout << "=== LIBRARY MANAGEMENT SYSTEM (LMS) - CORE FRAMEWORK INITIALIZED ===" << endl;
    
    // Khởi tạo thử nghiệm các đối tượng cơ bản
    User currentUser;
    Book sampleBook;
    BorrowTransaction currentTx;

    // Ví dụ gọi hàm kiểm tra tính sẵn sàng của sách (UC-05)
    if (sampleBook.checkAvailability()) {
        cout << "Sách hiện có sẵn để mượn." << endl;
    } else {
        cout << "Sách đã hết, gợi ý tính năng đặt trước (Reservation)." << endl;
    }

    return 0;
}
