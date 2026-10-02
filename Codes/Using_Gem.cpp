#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

// Class representing an individual Email
class Email {
private:
    string senderCategory;
    string subjectLine;
    string dateStr;
    int priorityRank;
    long dateValue; // Numeric value for easy date comparison (YYYYMMDD)

    // Convert date format MM-DD-YYYY to a comparable integer YYYYMMDD
    long parseDate(const string& d) {
        if (d.length() != 10) return 0;
        int month = stoi(d.substr(0, 2));
        int day = stoi(d.substr(3, 2));
        int year = stoi(d.substr(6, 4));
        return (year * 10000L) + (month * 100L) + day;
    }

    // Map sender category to a priority rank (higher number = higher priority)
    int getPriorityRank(const string& category) {
        if (category == "Boss") return 5;
        if (category == "Subordinate") return 4;
        if (category == "Peer") return 3;
        if (category == "ImportantPerson") return 2;
        if (category == "OtherPerson") return 1;
        return 0;
    }

public:
    Email() {
        senderCategory = "";
        subjectLine = "";
        dateStr = "";
        priorityRank = 0;
        dateValue = 0;
    }

    Email(string category, string subject, string date) {
        senderCategory = category;
        subjectLine = subject;
        dateStr = date;
        priorityRank = getPriorityRank(category);
        dateValue = parseDate(date);
    }

    string getSenderCategory() const { return senderCategory; }
    string getSubjectLine() const { return subjectLine; }
    string getDateStr() const { return dateStr; }

    // Comparison operator for MaxHeap:
    // Returns true if 'this' email has higher priority than 'other'.
    // Priority rules: 
    // 1. Higher sender category rank comes first.
    // 2. If categories are the same, newer date comes first.
    bool operator>(const Email& other) const {
        if (priorityRank != other.priorityRank) {
            return priorityRank > other.priorityRank;
        }
        return dateValue > other.dateValue;
    }

    bool operator<(const Email& other) const {
        if (priorityRank != other.priorityRank) {
            return priorityRank < other.priorityRank;
        }
        return dateValue < other.dateValue;
    }
};

// Class representing a List-based MaxHeap from scratch
class MaxHeap {
private:
    vector<Email> heap;

    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index] > heap[parent]) {
                Email temp = heap[index];
                heap[index] = heap[parent];
                heap[parent] = temp;
                index = parent;
            } else {
                break;
            }
        }
    }

    void siftDown(int index) {
        int size = heap.size();
        while (2 * index + 1 < size) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int largest = index;

            if (leftChild < size && heap[leftChild] > heap[largest]) {
                largest = leftChild;
            }
            if (rightChild < size && heap[rightChild] > heap[largest]) {
                largest = rightChild;
            }

            if (largest != index) {
                Email temp = heap[index];
                heap[index] = heap[largest];
                heap[largest] = temp;
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    MaxHeap() {}

    void insert(const Email& email) {
        heap.push_back(email);
        siftUp(heap.size() - 1);
    }

    bool isEmpty() const {
        return heap.empty();
    }

    int size() const {
        return heap.size();
    }

    Email peek() const {
        if (!isEmpty()) {
            return heap[0];
        }
        return Email();
    }

    void removeMax() {
        if (isEmpty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!isEmpty()) {
            siftDown(0);
        }
    }
};

// Main CEO Email Management System Object
class CEOInboxSystem {
private:
    MaxHeap emailHeap;

public:
    void addEmail(string category, string subject, string date) {
        Email newEmail(category, subject, date);
        emailHeap.insert(newEmail);
    }

    void displayCount() const {
        cout << "There are " << emailHeap.size() << " emails to read." << endl;
    }

    void displayNext() const {
        if (emailHeap.isEmpty()) {
            cout << "No emails in the inbox." << endl;
            return;
        }
        Email nextEmail = emailHeap.peek();
        cout << "Next email:" << endl;
        cout << "    Sender: " << nextEmail.getSenderCategory() << endl;
        cout << "    Subject: " << nextEmail.getSubjectLine() << endl;
        cout << "    Date: " << nextEmail.getDateStr() << endl;
    }

    void readEmail() {
        if (!emailHeap.isEmpty()) {
            emailHeap.removeMax();
        }
    }
};

// Driver code to parse command inputs
int main() {
    CEOInboxSystem inbox;

    // Example simulation commands matching the sample test file
    // In a real file-reading implementation, you would read line by line using ifstream.
    string commands[] = {
        "EMAIL Peer,Can you help me on this?,12-01-2024",
        "EMAIL OtherPerson,Try our product,12-19-2024",
        "EMAIL Boss,Important,12-20-2024",
        "EMAIL Subordinate,How do I handle this?,12-25-2024",
        "EMAIL ImportantPerson,Health Insurance Enrollment,12-31-2024",
        "EMAIL Boss,Never Mind,01-03-2025",
        "COUNT",
        "NEXT",
        "READ",
        "NEXT",
        "READ",
        "COUNT"
    };

    for (const string& cmd : commands) {
        if (cmd.rfind("EMAIL", 0) == 0) {
            // Parse comma-delimited fields: EMAIL <category>,<subject>,<date>
            size_t firstComma = cmd.find(',');
            size_t secondComma = cmd.find(',', firstComma + 1);

            string category = cmd.substr(6, firstComma - 6);
            string subject = cmd.substr(firstComma + 1, secondComma - firstComma - 1);
            string date = cmd.substr(secondComma + 1);

            inbox.addEmail(category, subject, date);
        } else if (cmd == "COUNT") {
            inbox.displayCount();
        } else if (cmd == "NEXT") {
            inbox.displayNext();
        } else if (cmd == "READ") {
            inbox.readEmail();
        }
    }

    return 0;
}