
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

const int MENU_SIZE = 8;

class FoodItem {
public:
    int id;
    string name;
    double price;
};

class OrderItem {
public:
    int foodId;
    string name;
    double price;
    int quantity;
    OrderItem* next;

    OrderItem(int id, string n, double p, int q) {
        foodId = id;
        name = n;
        price = p;
        quantity = q;
        next = nullptr;
    }
};

class Order {
public:
    int orderId;
    string customerName;
    OrderItem* items;
    double total;
    string status;
    Order* next;

    Order(int id, string customer) {
        orderId = id;
        customerName = customer;
        items = nullptr;
        total = 0;
        status = "Pending";
        next = nullptr;
    }

    void addItem(FoodItem food, int quantity) {
        OrderItem* temp = items;

        while (temp != nullptr) {
            if (temp->foodId == food.id) {
                temp->quantity += quantity;
                total += food.price * quantity;
                return;
            }
            temp = temp->next;
        }

        OrderItem* item = new OrderItem(
            food.id, food.name, food.price, quantity
        );

        item->next = items;
        items = item;
        total += food.price * quantity;
    }

    void display() const {
        cout << "\nOrder ID      : " << orderId;
        cout << "\nCustomer Name : " << customerName;
        cout << "\nStatus        : " << status;
        cout << "\n--------------- ORDER BILL ---------------\n";

        cout << left
             << setw(22) << "Food"
             << setw(10) << "Price"
             << setw(10) << "Qty"
             << "Amount\n";

        OrderItem* temp = items;

        while (temp != nullptr) {
            cout << left
                 << setw(22) << temp->name
                 << setw(10) << temp->price
                 << setw(10) << temp->quantity
                 << temp->price * temp->quantity << '\n';

            temp = temp->next;
        }

        cout << "-------------------------------------------\n";
        cout << "TOTAL BILL: Rs. "
             << fixed << setprecision(2) << total << '\n';
    }

    ~Order() {
        while (items != nullptr) {
            OrderItem* temp = items;
            items = items->next;
            delete temp;
        }
    }
};

class FoodOrderingSystem {
private:
    FoodItem menu[MENU_SIZE];
    Order* head;
    Order* tail;
    int nextOrderId;

    // Queue stores order IDs in FIFO order.
    // Cancelled or completed orders are skipped when served.
    int pendingQueue[1000];
    int front;
    int rear;

    void initializeMenu() {
        menu[0] = {1, "Burger", 80};
        menu[1] = {2, "Pizza", 200};
        menu[2] = {3, "Sandwich", 70};
        menu[3] = {4, "French Fries", 90};
        menu[4] = {5, "Pasta", 150};
        menu[5] = {6, "Cold Coffee", 100};
        menu[6] = {7, "Momos", 100};
        menu[7] = {8, "Veg Thali", 180};
    }

    FoodItem* findFood(int id) {
        for (int i = 0; i < MENU_SIZE; i++) {
            if (menu[i].id == id)
                return &menu[i];
        }
        return nullptr;
    }

    Order* findOrder(int id) {
        Order* temp = head;

        while (temp != nullptr) {
            if (temp->orderId == id)
                return temp;

            temp = temp->next;
        }

        return nullptr;
    }

    void enqueue(int id) {
        if (rear < 1000)
            pendingQueue[rear++] = id;
        else
            cout << "Queue is full!\n";
    }

public:
    FoodOrderingSystem() {
        head = nullptr;
        tail = nullptr;
        nextOrderId = 1001;
        front = 0;
        rear = 0;
        initializeMenu();
    }

    // 1. Display food menu
    void displayMenu() {
        cout << "\n========== FOOD MENU ==========\n";
        cout << left
             << setw(10) << "Food ID"
             << setw(22) << "Food Name"
             << "Price (Rs.)\n";

        for (int i = 0; i < MENU_SIZE; i++) {
            cout << left
                 << setw(10) << menu[i].id
                 << setw(22) << menu[i].name
                 << menu[i].price << '\n';
        }
    }

    // 2. Place an order
    void placeOrder() {
        string customer;
        int foodId, quantity;

        cout << "\nEnter Customer Name: ";
        getline(cin >> ws, customer);

        Order* order = new Order(nextOrderId, customer);

        while (true) {
            displayMenu();

            cout << "\nEnter Food ID (0 to finish): ";
            if (!(cin >> foodId)) {
                cin.clear();
                cin.ignore(
                    numeric_limits<streamsize>::max(), '\n'
                );
                cout << "Invalid input!\n";
                continue;
            }

            if (foodId == 0)
                break;

            FoodItem* food = findFood(foodId);

            if (food == nullptr) {
                cout << "Invalid Food ID!\n";
                continue;
            }

            cout << "Enter Quantity: ";
            if (!(cin >> quantity) || quantity <= 0) {
                cin.clear();
                cin.ignore(
                    numeric_limits<streamsize>::max(), '\n'
                );
                cout << "Quantity must be positive!\n";
                continue;
            }

            order->addItem(*food, quantity);
            cout << "Item added to order!\n";
        }

        if (order->items == nullptr) {
            cout << "No items selected. Order not placed.\n";
            delete order;
            return;
        }

        if (tail == nullptr) {
            head = tail = order;
        } else {
            tail->next = order;
            tail = order;
        }

        enqueue(nextOrderId);

        cout << "\nOrder placed successfully!\n";
        order->display();

        nextOrderId++;
    }

    // 3. Search an order
    void searchOrder() {
        int id;

        cout << "\nEnter Order ID: ";
        cin >> id;

        Order* order = findOrder(id);

        if (order == nullptr) {
            cout << "Order not found!\n";
            return;
        }

        order->display();
    }

    // 4. Display all orders
    void displayAllOrders() {
        if (head == nullptr) {
            cout << "\nNo orders found!\n";
            return;
        }

        Order* temp = head;

        while (temp != nullptr) {
            temp->display();
            temp = temp->next;
        }
    }

    // 5. Cancel a pending order
    void cancelOrder() {
        int id;

        cout << "\nEnter Order ID to cancel: ";
        cin >> id;

        Order* order = findOrder(id);

        if (order == nullptr) {
            cout << "Order not found!\n";
            return;
        }

        if (order->status != "Pending") {
            cout << "Only pending orders can be cancelled.\n";
            return;
        }

        order->status = "Cancelled";
        cout << "Order cancelled successfully!\n";
    }

    // 6. Process next pending order
    void processNextOrder() {
        while (front < rear) {
            int id = pendingQueue[front++];
            Order* order = findOrder(id);

            if (order == nullptr || order->status != "Pending")
                continue;

            order->status = "Completed";

            cout << "\nOrder processed successfully!\n";
            order->display();
            return;
        }

        cout << "No pending orders in the queue.\n";
    }

    // 7. Count orders
    void countOrders() {
        int total = 0;
        int pending = 0;
        int completed = 0;
        int cancelled = 0;

        Order* temp = head;

        while (temp != nullptr) {
            total++;

            if (temp->status == "Pending")
                pending++;
            else if (temp->status == "Completed")
                completed++;
            else if (temp->status == "Cancelled")
                cancelled++;

            temp = temp->next;
        }

        cout << "\nTotal Orders     : " << total;
        cout << "\nPending Orders   : " << pending;
        cout << "\nCompleted Orders : " << completed;
        cout << "\nCancelled Orders : " << cancelled << '\n';
    }

    // Free allocated memory
    ~FoodOrderingSystem() {
        while (head != nullptr) {
            Order* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main() {
    FoodOrderingSystem system;
    int choice;

    do {
        cout << "\n\n===================================";
        cout << "\n     FOOD ORDERING MANAGEMENT";
        cout << "\n===================================";
        cout << "\n1. Display Food Menu";
        cout << "\n2. Place Order";
        cout << "\n3. Search Order";
        cout << "\n4. Display All Orders";
        cout << "\n5. Cancel Order";
        cout << "\n6. Process Next Order";
        cout << "\n7. Count Orders";
        cout << "\n0. Exit";
        cout << "\nEnter Your Choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );
            continue;
        }

        switch (choice) {
            case 1:
                system.displayMenu();
                break;
            case 2:
                system.placeOrder();
                break;
            case 3:
                system.searchOrder();
                break;
            case 4:
                system.displayAllOrders();
                break;
            case 5:
                system.cancelOrder();
                break;
            case 6:
                system.processNextOrder();
                break;
            case 7:
                system.countOrders();
                break;
            case 0:
                cout << "Thank you for using Food Ordering System!\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
