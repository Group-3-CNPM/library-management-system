#include <iostream>
#include <string>
#include <vector>

using namespace std;
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
//Viết lần lượt các câu lệnh sau vào terminal để lưu lại trên github
//1 Viết git add . để lưu thay đổi
//2 git commit -m "" viết vào "" nội dung thay đổi như "sửa hàm chức năng trong class User"
//3 git push origin main để lưu trên github