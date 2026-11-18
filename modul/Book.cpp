#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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
    Book() {
        this->bookID = 0;
        this->isbn = "";
        this->title = "";
        this->author = "";
        this->category = "";
        this->quantity = 0;
    }

    Book(int id, string isbn, string title, string author, string category, int qty) {
        this->bookID = id;
        this->isbn = isbn;
        this->title = title;
        this->author = author;
        this->category = category;
        this->quantity = qty;
    }

    int getBookID() const { 
        return bookID; 
    }

    string getIsbn() const { 
        return isbn; 
    }
    
    string getTitle() const { 
        return title; 
    }

    string getAuthor() const { 
        return author; 
    }

    string getCategory() const { 
        return category; 
    }

    int getQuantity() const { 
        return quantity; 
    }

    void setQuantity(int qty) { 
        this->quantity = qty; 
    }

    // 1. Thêm sách mới vào danh mục (FR2, FR16)
    bool addBook(Book bookData) {
        // Thêm đầu sách mới vào CSDL (FR2, FR16)
        if (bookData.isbn.empty() || bookData.title.empty()) {
            cout << "Lỗi: ISBN và Tiêu đề sách không được để trống!" << endl;
            return false;
        }
        // Viết câu lệnh SQL cập nhập thông tin sách mới vào data bsse
        cout << "Đã thêm thành công đầu sách: " << bookData.title << " (ISBN: " << bookData.isbn << ")" << endl;
        return true;
    }

    bool updateBook(int id, Book bookData) {
        // Cập nhật thông tin sách (tiêu đề, tác giả, v.v.)
        if (id <= 0) {
            cout << "Lỗi: ID sách không hợp lệ!" << endl;
            return false;
        }
        this->isbn = bookData.isbn;
        this->title = bookData.title;
        this->author = bookData.author;
        this->category = bookData.category;
        this->quantity = bookData.quantity;
        // Viết câu lệnh SQL cập nhập lại thông  tin sách trong data base
        cout << "Cập nhật thành công thông tin cho Book ID: " << id << endl;
        return true;
    }

    bool removeBook(int id) {
        // Xóa hoặc ẩn sách hỏng/cũ khỏi danh mục (FR2)
        // Viết câu lệnh SQL Kiểm tra xem sách có đang có giao dịch mượn chưa trả hay không trước khi xóa.
        cout << "Đã xóa thành công Book ID: " << id << " khỏi hệ thống." << endl;
        return true;
    }

    vector<Book> searchBook(string keyword) {
        // Tìm kiếm sách theo tiêu đề, tác giả, ISBN hoặc thể loại (FR3)
        vector<Book> searchResults;
        if (keyword.empty()) {
            cout << "Lỗi: Từ khóa tìm kiếm không được để trống!" << endl;
            return searchResults;
        }
        //Viết câu lệnh SQL tìm những sách có keyword trùng với keyword do user nhập
        cout << "Đang thực hiện tìm kiếm với từ khóa: \"" << keyword << "\"" << endl;
        return searchResults;
    }

    bool checkAvailability() {
        // Kiểm tra số lượng quantity > 0 hay không (UC-05)
        return (this->quantity > 0);
    }

    void updateQuantity(int changeAmount) {
        // Tăng/giảm số lượng tồn kho khi mượn hoặc trả sách (FR4, FR5)
        this->quantity += changeAmount;
        if (this->quantity < 0) {
            this->quantity = 0;
        }
        //Viết câu lệnh SQL cập nhập lại số lượng của ID sách do user nhập trong data base 
        cout << "Đã cập nhật số lượng tồn kho của sách ID " << this->bookID << ". Số lượng mới: " << this->quantity << endl;
    }

    Book getBookDetails() {
        // Trả về thông tin chi tiết của sách (UC-03)
        cout << "=== CHI TIẾT SÁCH ===" << endl;
        cout << "Book ID: " << this->bookID << endl;
        cout << "ISBN: " << this->isbn << endl;
        cout << "Tiêu đề: " << this->title << endl;
        cout << "Tác giả: " << this->author << endl;
        cout << "Thể loại: " << this->category << endl;
        cout << "Số lượng khả dụng: " << this->quantity << endl;
        return *this;
    }
};
