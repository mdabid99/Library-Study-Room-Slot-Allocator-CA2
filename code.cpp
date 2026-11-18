// CAP455 CA2 - Library Study-Room Slot Allocator (simple version)
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const char* FILE_NAME = "C:\\Users\\abidm\\Downloads\\bookings.dat";

class Booking {
    int id, room, slot, day;
    char name[20];                      // fixed size, so every record has the same size
    static int alive;                   // static: objects currently in memory
public:
    Booking() {                         // default constructor
        id = room = slot = day = 0;
        name[0] = '\0';
        alive++;
    }
    Booking(int id, const char* name, int room, int slot, int day) {  // parameterised
        this->id = id;
        strncpy(this->name, name, 19);
        this->name[19] = '\0';
        this->room = room;
        this->slot = slot;
        this->day = day;
        alive++;
    }
    Booking(const Booking& b) {         // copy constructor
        id = b.id;
        strcpy(name, b.name);
        room = b.room;
        slot = b.slot;
        day = b.day;
        alive++;
    }
    ~Booking() { alive--; }             // destructor

    int getId() const { return id; }    // inline function
    void changeSlot(int r, int s, int d) { room = r; slot = s; day = d; }
    void show() const {
        cout << "ID " << id << " | " << name << " | Room " << room
             << " | Slot " << slot << " | Day " << day << endl;
    }
    static int getAlive() { return alive; }
    friend bool clash(const Booking& b, int room, int slot, int day);
};
int Booking::alive = 0;

bool clash(const Booking& b, int room, int slot, int day) {   // friend function
    return b.room == room && b.slot == slot && b.day == day;
}

// ---------- file helpers ----------
int countRecords() {                    // records = file size / size of one record
    ifstream f(FILE_NAME, ios::binary);
    if (!f) return 0;
    f.seekg(0, ios::end);
    long size = f.tellg();
    return size / sizeof(Booking);
}

Booking readAt(int i) {                 // seekg jumps straight to record i
    Booking b;
    ifstream f(FILE_NAME, ios::binary);
    f.seekg(i * sizeof(Booking), ios::beg);
    f.read((char*)&b, sizeof(b));
    return b;
}

int findIndex(int id) {                 // record number of this ID, or -1
    for (int i = 0; i < countRecords(); i++)
        if (readAt(i).getId() == id) return i;
    return -1;
}

// returns an error message, or nullptr if the booking is fine
const char* check(int room, int slot, int day, int skipId) {
    if (room < 1 || room > 5 || slot < 1 || slot > 6 || day < 1 || day > 31)
        return "Invalid room/slot/day.";
    for (int i = 0; i < countRecords(); i++) {
        Booking b = readAt(i);
        if (b.getId() != skipId && clash(b, room, slot, day))
            return "Room already booked.";
    }
    return nullptr;
}

// ---------- menu actions ----------
void addBooking() {
    int id, room, slot, day;
    char name[20];
    cout << "ID: ";          cin >> id;
    cout << "Name: ";        cin.width(20); cin >> name;
    cout << "Room (1-5): ";  cin >> room;
    cout << "Slot (1-6): ";  cin >> slot;
    cout << "Day (1-31): ";  cin >> day;

    if (id <= 0 || findIndex(id) != -1) { cout << "Bad or duplicate ID.\n"; return; }
    const char* msg = check(room, slot, day, 0);
    if (msg) { cout << msg << endl; return; }

    Booking b(id, name, room, slot, day);
    ofstream f(FILE_NAME, ios::binary | ios::app);   // add at end of file
    f.write((char*)&b, sizeof(b));
    cout << "Booking added.\n";
}

void viewAll() {
    for (int i = 0; i < countRecords(); i++) readAt(i).show();
    cout << "Total records: " << countRecords() << endl;
    cout << "Objects alive: " << Booking::getAlive() << endl;
}

void searchBooking() {
    int id;
    cout << "ID to search: ";  cin >> id;
    int i = findIndex(id);
    if (i == -1) { cout << "Not found.\n"; return; }
    cout << "Found at record " << i << endl;
    readAt(i).show();
}

void updateBooking() {
    int id, room, slot, day;
    cout << "ID to update: ";  cin >> id;
    int i = findIndex(id);
    if (i == -1) { cout << "Not found.\n"; return; }

    Booking b = readAt(i);              // copy constructor
    b.show();
    cout << "New room (1-5): ";  cin >> room;
    cout << "New slot (1-6): ";  cin >> slot;
    cout << "New day (1-31): ";  cin >> day;

    const char* msg = check(room, slot, day, id);
    if (msg) { cout << msg << endl; return; }

    b.changeSlot(room, slot, day);
    fstream f(FILE_NAME, ios::in | ios::out | ios::binary);
    f.seekp(i * sizeof(Booking), ios::beg);          // jump to that record
    f.write((char*)&b, sizeof(b));                   // overwrite it
    cout << "Updated.\n";
}

int main() {
    int choice;
    do {
        cout << "\n1.Add  2.View all  3.Search  4.Update  0.Exit : ";
        cin >> choice;
        if      (choice == 1) addBooking();
        else if (choice == 2) viewAll();
        else if (choice == 3) searchBooking();
        else if (choice == 4) updateBooking();
    } while (choice != 0);
    return 0;
}