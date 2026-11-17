#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <cstdlib>
using namespace std;

#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"
#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

struct Product { int id; string name, category; int size; double price; int stock; };
struct CartItem { int productId, quantity; };

vector<Product> products = {
    {1001, "Air Force 1 '07", "Lifestyle", 8, 5495.00, 12},
    {1002, "Air Max 90", "Lifestyle", 9, 6895.00, 8},
    {1003, "Dunk Low Retro", "Basketball", 8, 6295.00, 10},
    {1004, "Pegasus 41", "Running", 9, 7495.00, 6},
    {1005, "Jordan 1 Low", "Basketball", 10, 6995.00, 5},
    {1006, "Metcon 9", "Training", 8, 7295.00, 4}
};
int nextId = 1007, completedOrders = 0;
double totalSales = 0.0;

void warn(const string& msg) { cout << RED << msg << RESET << "\n"; }

void pauseScreen() { cout << "\nPress Enter to continue..."; string d; getline(cin, d); }

void line(char c = '=') { cout << (c == '=' ? BLUE : "") << string(60, c) << RESET << "\n"; }

void header(const string& title, bool menu = false) {
    system(CLEAR);
    if (menu) {
        line();
        cout << BLUE << "                 NIKE ONLINE STORE WELCOME\n" << RESET
             << "                    " << title << "\n";
        line();
    } else cout << BLUE << title << RESET << "\n\n";
}

void menuItem(const string& key, const string& text) { cout << GREEN << key << "." << RESET << " " << text << "\n"; }

string readText(const string& prompt) { string v; cout << prompt; getline(cin, v); return v; }

template <class T> T readNum(const string& prompt, T minimum = 0) {
    while (true) {
        string input; T value; char extra;
        cout << prompt; getline(cin, input);
        stringstream ss(input);
        if (ss >> value && !(ss >> extra) && value >= minimum) return value;
        ostringstream o; o << "Please enter a valid number of at least " << minimum << ".";
        warn(o.str());
    }
}

string lower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)tolower(c); });
    return s;
}

int findProductIndex(int id) {
    for (int i = 0; i < (int)products.size(); ++i) if (products[i].id == id) return i;
    return -1;
}

int askProduct(const string& prompt) {
    int i = findProductIndex(readNum<int>(prompt, 1));
    if (i == -1) warn("Product ID not found.");
    return i;
}

void printProductTable(const vector<Product>& list) {
    if (list.empty()) return warn("No products found.");
    cout << left << setw(7) << "ID" << setw(25) << "PRODUCT" << setw(15) << "CATEGORY"
         << right << setw(7) << "SIZE" << setw(13) << "PRICE (PHP)" << setw(9) << "STOCK" << "\n";
    line('-');
    for (const Product& p : list)
        cout << left << setw(7) << p.id << setw(25) << p.name.substr(0, 23) << setw(15) << p.category.substr(0, 13)
             << right << setw(7) << p.size << setw(13) << p.price << setw(9) << p.stock << "\n";
}

void dashboard() {
    int units = 0, lowStock = 0;
    double value = 0.0;
    for (const Product& p : products) { units += p.stock; value += p.price * p.stock; lowStock += p.stock <= 5; }
    header("ADMIN DASHBOARD");
    cout << "Products listed       : " << products.size() << "\n"
         << "Total units in stock  : " << units << "\n"
         << "Low-stock products    : " << lowStock << "\n"
         << "Inventory value       : PHP " << value << "\n"
         << "Completed orders      : " << completedOrders << "\n"
         << "Total sales           : PHP " << totalSales << "\n";
}

void viewProducts() { header("PRODUCT INVENTORY"); printProductTable(products); }

void addProduct() {
    header("ADD NEW NIKE PRODUCT");
    Product p;
    p.name = readText("Product name: ");
    if (p.name.empty()) return warn("Product name cannot be empty. Product was not added.");
    p.id = nextId++;
    p.category = readText("Category (Lifestyle/Running/Basketball/Training): ");
    if (p.category.empty()) p.category = "Other";
    p.size = readNum<int>("Shoe size: ", 1);
    p.price = readNum<double>("Price in PHP: ", 0.01);
    p.stock = readNum<int>("Starting stock quantity: ", 0);
    products.push_back(p);
    cout << "Product added successfully! Product ID: " << p.id << "\n";
}

void editText(const string& label, string& field) {
    string v = readText("New " + label + " [" + field + "]: ");
    if (!v.empty()) field = v;
}

template <class T> void editNum(const string& label, T& field, T minimum, bool strict) {
    cout << "New " << label << " [" << field << "]: ";
    string v; getline(cin, v);
    if (v.empty()) return;
    stringstream ss(v); T n; char extra;
    if (ss >> n && !(ss >> extra) && (strict ? n > minimum : n >= minimum)) field = n;
    else warn("Invalid " + label + ". Keeping old value.");
}

void updateProduct() {
    header("UPDATE PRODUCT");
    int i = askProduct("Enter product ID: ");
    if (i == -1) return;
    Product& p = products[i];
    cout << "\nCurrent product details:\n";
    printProductTable(vector<Product>{p});
    cout << "\nPress Enter to keep the current value.\n";
    editText("product name", p.name);
    editText("category", p.category);
    editNum("shoe size", p.size, 0, true);
    editNum("price", p.price, 0.0, true);
    editNum("stock quantity", p.stock, 0, false);
    cout << "Product updated successfully.\n";
}

void deleteProduct() {
    header("DELETE PRODUCT");
    int i = askProduct("Enter product ID to delete: ");
    if (i == -1) return;
    string confirm = readText("Delete " + products[i].name + "? (Y/N): ");
    if (confirm == "Y" || confirm == "y") { products.erase(products.begin() + i); cout << "Product deleted successfully.\n"; }
    else warn("Delete cancelled.");
}

void searchProduct() {
    header("SEARCH PRODUCTS");
    string key = lower(readText("Enter product name or category: "));
    if (key.empty()) return warn("Search cannot be empty.");
    vector<Product> matches;
    for (const Product& p : products) if (lower(p.name + " " + p.category).find(key) != string::npos) matches.push_back(p);
    printProductTable(matches);
}

void restockProduct() {
    header("RESTOCK INVENTORY");
    int i = askProduct("Enter product ID: ");
    if (i == -1) return;
    products[i].stock += readNum<int>("Quantity to add: ", 1);
    cout << "Restock successful. New stock: " << products[i].stock << "\n";
}

void showLowStock() {
    header("LOW-STOCK ALERT");
    vector<Product> low;
    for (const Product& p : products) if (p.stock <= 5) low.push_back(p);
    printProductTable(low);
    cout << "\nLow-stock threshold: 5 units or fewer.\n";
}

void addToCart(vector<CartItem>& cart) {
    int i = askProduct("Enter product ID: ");
    if (i == -1) return;
    int id = products[i].id;
    auto it = find_if(cart.begin(), cart.end(), [&](const CartItem& c) { return c.productId == id; });
    int available = products[i].stock - (it != cart.end() ? it->quantity : 0);
    if (available <= 0) return warn("No more stock available for this product.");
    int q = readNum<int>("Quantity to add: ", 1);
    if (q > available) return warn("Only " + to_string(available) + " unit(s) available.");
    if (it != cart.end()) it->quantity += q; else cart.push_back({id, q});
    cout << "Added to cart.\n";
}

void viewCart(const vector<CartItem>& cart) {
    header("YOUR SHOPPING CART");
    double subtotal = 0.0;
    if (cart.empty()) cout << "Your cart is empty.\n";
    for (const CartItem& c : cart) {
        int i = findProductIndex(c.productId);
        if (i == -1) continue;
        double amount = products[i].price * c.quantity;
        cout << products[i].name << " x " << c.quantity << " = PHP " << amount << "\n";
        subtotal += amount;
    }
    cout << "Subtotal: PHP " << subtotal << "\n";
}

void checkout(vector<CartItem>& cart) {
    if (cart.empty()) return warn("Your cart is empty. Add products first.");
    double subtotal = 0.0;
    for (const CartItem& c : cart) {
        int i = findProductIndex(c.productId);
        if (i == -1 || products[i].stock < c.quantity) return warn("Stock changed. Please review your cart.");
        subtotal += products[i].price * c.quantity;
    }
    header("CHECKOUT");
    cout << "Order subtotal: PHP " << subtotal << "\n1. Cash on Delivery\n2. E-wallet (demo)\n3. Bank Transfer (demo)\n";
    int payment = readNum<int>("Choose payment method: ", 1);
    if (payment > 3) return warn("Invalid payment method.");
    string customer = readText("Customer name: ");
    if (customer.empty()) customer = "Walk-in Customer";
    for (const CartItem& c : cart) products[findProductIndex(c.productId)].stock -= c.quantity;
    totalSales += subtotal;
    ++completedOrders;
    const char* methods[] = {"Cash on Delivery", "E-wallet (demo)", "Bank Transfer (demo)"};
    cout << "\nORDER CONFIRMED!\nCustomer: " << customer << "\nOrder total: PHP " << subtotal
         << "\nPayment: " << methods[payment - 1] << "\nThank you for shopping at the Nike Online Store!\n";
    cart.clear();
}

void customerStore() {
    vector<CartItem> cart;
    while (true) {
        header("SHOP NIKE", true);
        printProductTable(products);
        cout << "\n1. Add product to cart\n2. View cart\n3. Checkout\n0. Return to main menu\n";
        int choice = readNum<int>("Choose an option: ");
        if (choice == 0) return;
        if (choice == 1) addToCart(cart);
        else if (choice == 2) viewCart(cart);
        else if (choice == 3) checkout(cart);
        else warn("Invalid option. Please choose again.");
        pauseScreen();
    }
}

void adminMenu() {
    const char* labels[] = {"View all products", "Add new product", "Update product", "Delete product",
                            "Search product", "Restock product", "Low-stock alerts", "Dashboard"};
    void (*actions[])() = {viewProducts, addProduct, updateProduct, deleteProduct,
                           searchProduct, restockProduct, showLowStock, dashboard};
    while (true) {
        header("ADMIN INVENTORY MENU", true);
        for (int i = 0; i < 8; ++i) menuItem(to_string(i + 1), labels[i]);
        menuItem("0", "Return to main menu");
        int choice = readNum<int>("Choose an option: ");
        if (choice == 0) return;
        if (choice >= 1 && choice <= 8) actions[choice - 1](); else warn("Invalid option. Please choose again.");
        pauseScreen();
    }
}

int main() {
    cout << fixed << setprecision(2);
    while (true) {
        header("WELCOME", true);
        menuItem("1", "Shop Nike products");
        menuItem("2", "Admin / Inventory management");
        menuItem("3", "View dashboard");
        menuItem("0", "Exit");
        int choice = readNum<int>("Choose an option: ");
        if (choice == 0) {
            system(CLEAR);
            cout << BLUE << "\n\n" << string(60, '=') << "\n          THANK YOU FOR VISITING NIKE ONLINE STORE!\n"
                 << string(60, '=') << "\n" << RESET;
            return 0;
        }
        if (choice == 1) customerStore();
        else if (choice == 2) adminMenu();
        else if (choice == 3) { dashboard(); pauseScreen(); }
        else { warn("Invalid option. Please choose again."); pauseScreen(); }
    }
}
