#include <iostream>
#include <string>
#include <vector>

using namespace std;
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
//Viết lần lượt các câu lệnh sau vào terminal để lưu lại trên github
//1 Viết git add modul để lưu thay đổi
//2 git commit -m "" viết vào "" nội dung thay đổi như "sửa hàm chức năng trong class User"
//3 git push origin main để lưu trên github