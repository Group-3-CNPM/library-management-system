#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <string>

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
    User(int id, string name, string mail, string pass, string r, string status)
    {
        this->userID = id;
        this->userName = name;
        this->email = mail;
        this->password = pass;
        this->role = r;
        this->accountStatus = status;
    }
    
    int getUserID() const
    {
        return userID;
    }

    string getUserName() const
    {
        return userName;
    }

    string getEmail() const
    {
        return email;
    }

    string getRole() const
    {
        return role;
    }

    string getAccountStatus() const
    {
        return accountStatus;
    }
    // Các hàm chức năng chính
    bool login(string name, string pass) {
        //Viết câu lệnh truy vấn từ Data base của SQL

        // yêu cầu nhập username và password
        if(name.empty() || pass.empty()) {
        cout << "Lỗi: Tên đăng nhập và mật khẩu không được để trống!" << endl;
        return false;
        }
        // kiểm tra trạng thái tài khoản
        if (this->accountStatus == "Suspended") {
        cout << "Lỗi: Tài khoản của bạn đang bị đình chỉ (Suspended) do vi phạm hoặc nợ phạt!" << endl;
        return false;
        }
        // login thành công
        if (this->userName == name && this->password == pass) {
        cout << "Đăng nhập thành công! Chào mừng " << this->userName << " (" << this->role << ")" << endl;
        return true;
        }
        cout << "Lỗi: Sai tên đăng nhập hoặc mật khẩu!" << endl;
        return false;
    }

    void logout() {
        // logout thành công
        cout << "Người dùng " << this->userName << " đã đăng xuất thành công." << endl;
    }

    bool updateProfile(string newName, string newMail) {
        //Cập nhật thông tin cá nhân của Member/Librarian
        if (newName.empty() || newMail.empty()) {
        cout << "Lỗi: Thông tin cập nhật không hợp lệ!" << endl;
        return false;
        }
        this->userName = newName;
        this->email = newMail;
        //Câu lệnh SQL tìm đến ID tài khoản cần thay đổi và đổi thông tin tài khoản
        cout << "Cập nhật thông tin cá nhân thành công cho User ID: " << this->userID << endl;
        return true;
    }

    bool changePassword(string oldPass, string newPass) {
        //Xác thực mật khẩu cũ và lưu mật khẩu mới đã mã hóa
        if (oldPass != this->password) {
        cout << "Lỗi: Mật khẩu cũ không chính xác!" << endl;
        return false;
        }
        if (newPass.length() < 6) {
        cout << "Lỗi: Mật khẩu mới phải có ít nhất 6 ký tự!" << endl;
        return false;
        }
        //Câu lệnh SQL kiểm tra mật khẩu mới có trùng với mật khẩu cũ không
        this->password = newPass;
        cout << "Đổi mật khẩu thành công!" << endl;
        return true;
    }

    bool addUser(User userData) {
        //Admin/Librarian thêm tài khoản mới 
        if (this->role != "Administrator" && this->role != "Librarian") {
        cout << "Lỗi: Bạn không có quyền thực hiện thao tác này!" << endl;
        return false;
        }
        //Câu lệnh SQL để thêm user mới
        cout << "Đã thêm thành công người dùng mới: " << userData.getUserName() << endl;
        return true;
    }

    bool updateUser(int id, User userData) {
        //Cập nhật thông tin tài khoản theo ID
        if (this->role != "Administrator" && this->role != "Librarian") {
        cout << "Lỗi: Quyền hạn hiện tại không cho phép!" << endl;
        return false;
    }
        //Câu lệnh SQL để thay đổi quyền hạn của user trong data base
        cout << "Đã cập nhật thông tin cho User ID: " << id << endl;
        return true;
    }

    bool deactivateUser(int id) {
        //Vô hiệu hóa tài khoản vi phạm hoặc có phạt chưa trả (FR8)
        if (this->role != "Administrator") {
        cout << "Lỗi: Chỉ có Administrator mới có quyền xóa hoặc vô hiệu hóa tài khoản khác!" << endl;
        return false;
    }
        //Câu lệnh SQL để đổi trạng tài khoản của user thành Suspended( vô hiệu hóa)
        cout << "Đã vô hiệu hóa thành công tài khoản có User ID: " << id << endl;
        return true;
    }

    User showProfile() {
        //Trả về thông tin chi tiết của người dùng
        cout << "=== THÔNG TIN TÀI KHOẢN ===" << endl;
        cout << "User ID: " << this->userID << endl;
        cout << "Tên: " << this->userName << endl;
        cout << "Email: " << this->email << endl;
        cout << "Vai trò: " << this->role << endl;
        cout << "Trạng thái: " << this->accountStatus << endl;
        return *this;
    }
};
