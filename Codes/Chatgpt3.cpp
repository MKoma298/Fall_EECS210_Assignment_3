#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// ---------------------------------------------------------
// Email class
// ---------------------------------------------------------
class Email
{
private:
    string sender;
    string subject;
    string date;

public:
    Email()
    {
        sender = "";
        subject = "";
        date = "";
    }

    Email(string s, string sub, string d)
    {
        sender = s;
        subject = sub;
        date = d;
    }

    string getSender()
    {
        return sender;
    }

    string getSubject()
    {
        return subject;
    }

    string getDate()
    {
        return date;
    }

    int getPriority()
    {
        if (sender == "Boss")
            return 5;
        else if (sender == "Subordinate")
            return 4;
        else if (sender == "Peer")
            return 3;
        else if (sender == "ImportantPerson")
            return 2;
        else
            return 1;
    }

    // Convert MM-DD-YYYY into YYYYMMDD
    // This makes date comparisons easy.
    int getDateValue()
    {
        int month = stoi(date.substr(0, 2));
        int day = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        return year * 10000 + month * 100 + day;
    }
};


// ---------------------------------------------------------
// Node class
// ---------------------------------------------------------
class HeapNode
{
public:
    Email email;
    HeapNode* next;

    HeapNode(Email e)
    {
        email = e;
        next = nullptr;
    }
};


// ---------------------------------------------------------
// MaxHeap class
// ---------------------------------------------------------
class MaxHeap
{
private:
    HeapNode* head;
    int size;

    // Return the node at a particular position.
    // Position 1 = root
    HeapNode* getNode(int position)
    {
        HeapNode* current = head;

        for (int i = 1; i < position && current != nullptr; i++)
        {
            current = current->next;
        }

        return current;
    }

    // Determines whether email1 has higher priority
    // than email2.
    bool higherPriority(Email email1, Email email2)
    {
        if (email1.getPriority() > email2.getPriority())
        {
            return true;
        }

        if (email1.getPriority() < email2.getPriority())
        {
            return false;
        }

        // Same sender category:
        // newer date gets priority.
        return email1.getDateValue() > email2.getDateValue();
    }

    void swapEmails(HeapNode* first, HeapNode* second)
    {
        Email temp = first->email;
        first->email = second->email;
        second->email = temp;
    }

    // Move a newly inserted node upward.
    void heapifyUp(int position)
    {
        while (position > 1)
        {
            int parentPosition = position / 2;

            HeapNode* current = getNode(position);
            HeapNode* parent = getNode(parentPosition);

            if (higherPriority(current->email, parent->email))
            {
                swapEmails(current, parent);
                position = parentPosition;
            }
            else
            {
                break;
            }
        }
    }

    // Move the root downward after removing it.
    void heapifyDown()
    {
        int position = 1;

        while (true)
        {
            int leftPosition = position * 2;
            int rightPosition = position * 2 + 1;

            if (leftPosition > size)
            {
                break;
            }

            int largerChildPosition = leftPosition;

            HeapNode* leftChild = getNode(leftPosition);

            if (rightPosition <= size)
            {
                HeapNode* rightChild = getNode(rightPosition);

                if (higherPriority(rightChild->email,
                                    leftChild->email))
                {
                    largerChildPosition = rightPosition;
                }
            }

            HeapNode* current = getNode(position);
            HeapNode* largerChild = getNode(largerChildPosition);

            if (higherPriority(largerChild->email,
                               current->email))
            {
                swapEmails(current, largerChild);
                position = largerChildPosition;
            }
            else
            {
                break;
            }
        }
    }

public:

    MaxHeap()
    {
        head = nullptr;
        size = 0;
    }

    ~MaxHeap()
    {
        while (head != nullptr)
        {
            HeapNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insert(Email email)
    {
        HeapNode* newNode = new HeapNode(email);

        if (head == nullptr)
        {
            head = newNode;
            size = 1;
            return;
        }

        // Add to the end of the linked list.
        HeapNode* current = head;

        while (current->next != nullptr)
        {
            current = current->next;
        }

        current->next = newNode;
        size++;

        heapifyUp(size);
    }

    Email getMax()
    {
        return head->email;
    }

    void removeMax()
    {
        if (head == nullptr)
        {
            return;
        }

        // If there is only one email.
        if (size == 1)
        {
            delete head;
            head = nullptr;
            size = 0;
            return;
        }

        // Find the last node.
        HeapNode* previous = nullptr;
        HeapNode* current = head;

        while (current->next != nullptr)
        {
            previous = current;
            current = current->next;
        }

        // Move last email to root.
        head->email = current->email;

        // Remove last node.
        previous->next = nullptr;
        delete current;

        size--;

        heapifyDown();
    }

    int getSize()
    {
        return size;
    }

    bool isEmpty()
    {
        return size == 0;
    }
};


// ---------------------------------------------------------
// CEO Inbox class
// ---------------------------------------------------------
class CEOInbox
{
private:
    MaxHeap inbox;

public:

    void receiveEmail(string sender, string subject, string date)
    {
        Email email(sender, subject, date);
        inbox.insert(email);
    }

    void nextEmail()
    {
        if (inbox.isEmpty())
        {
            return;
        }

        Email email = inbox.getMax();

        cout << "Next email:" << endl;
        cout << "    Sender: " << email.getSender() << endl;
        cout << "    Subject: " << email.getSubject() << endl;
        cout << "    Date: " << email.getDate() << endl;
        cout << endl;
    }

    void readEmail()
    {
        if (!inbox.isEmpty())
        {
            inbox.removeMax();
        }
    }

    void countEmails()
    {
        cout << "There are " << inbox.getSize()
             << " emails to read." << endl;
        cout << endl;
    }
};


// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
int main()
{
    ifstream inputFile("test.txt");

    if (!inputFile)
    {
        cout << "Error opening test file." << endl;
        return 1;
    }

    CEOInbox inbox;

    string line;

    while (getline(inputFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        // ---------------------------------------------
        // EMAIL command
        // ---------------------------------------------
        if (line.substr(0, 5) == "EMAIL")
        {
            // Remove "EMAIL "
            string emailData = line.substr(6);

            stringstream ss(emailData);

            string sender;
            string subject;
            string date;

            // Sender
            getline(ss, sender, ',');

            // Subject
            getline(ss, subject, ',');

            // Date
            getline(ss, date);

            inbox.receiveEmail(sender, subject, date);
        }

        // ---------------------------------------------
        // NEXT command
        // ---------------------------------------------
        else if (line == "NEXT")
        {
            inbox.nextEmail();
        }

        // ---------------------------------------------
        // READ command
        // ---------------------------------------------
        else if (line == "READ")
        {
            inbox.readEmail();
        }

        // ---------------------------------------------
        // COUNT command
        // ---------------------------------------------
        else if (line == "COUNT")
        {
            inbox.countEmails();
        }
    }

    inputFile.close();

    return 0;
}