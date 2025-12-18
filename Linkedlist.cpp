#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    size_t count = 0;

    // Helper to get raw pointer to node at position (0-based)
    Node* get_node(size_t pos) const;

public:
    LinkedList() = default;

    // Rule of Five
    LinkedList(const LinkedList& other) : count(other.count) {
        if (!other.head) {
            return; // copying an empty list
        }

        // Step 1: copy the head
        head = std::make_unique<Node>(other.head->data);

        Node* thisCur = head.get();
        Node* otherCur = other.head->next.get();

        // Step 2: copy remaining nodes
        while (otherCur) {
            thisCur->next = std::make_unique<Node>(otherCur->data);
            thisCur = thisCur->next.get();
            otherCur = otherCur->next.get();
        }
    }
    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) {return *this;}

        if (!other.head) {return;}
        
        delete head;
        delete next; 

        head = std::make_unique<Node>(other.head->data);

        Node* thisCur = head.get();
        Node* otherCur = other.head->next.get();

        // Step 2: copy remaining nodes
        while (otherCur) {
            thisCur->next = std::make_unique<Node>(otherCur->data);
            thisCur = thisCur->next.get();
            otherCur = otherCur->next.get();
        }
        return *this;

    }

    
    LinkedList(LinkedList&& other) noexcept;
    LinkedList& operator=(LinkedList&& other) noexcept;
    ~LinkedList() = default;

    // Core operations
    void push_front(const T& value) {
        // Create new node
        auto newNode = std::make_unique<Node>(value);

        // Transfer ownership of current head to newNode->next
        newNode->next = std::move(head);

        // New node becomes the head
        head = std::move(newNode);

        ++count;
    }

    void push_back(const T& value) {
        auto newNode = std::make_unique<Node>(value);
    
        // Start from the head unique_ptr
        std::unique_ptr<Node>* curr = &head;
    
        // Traverse until we find a null unique_ptr
        while ((*curr)) {
            curr = &((*curr)->next);
        }
    
        // Attach the new node
        *curr = std::move(newNode);
    
        ++count;
    }
    
    
    T pop_front() {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
    
        T value = head->data;              // ① copy value FIRST
        head = std::move(head->next);      // ② destroy old head
        --count;
        return value;        
    }

    T pop_back() {
        if (!head) {
            throw std::out_of_range("List is empty");
        }
    
        std::unique_ptr<Node>* curr = &head;
    
        // Move curr until it points to the LAST node's owner
        while ((*curr)->next) {
            curr = &((*curr)->next);
        }
    
        T value = (*curr)->data;  // copy or move data
        *curr = nullptr;                     // destroy last node
        --count;
    
        return value;
    }

    void insert(size_t pos, const T& value) {
        if (pos >= count) {
            throw std::out_of_range("Invalid position");
        }
    
        if (pos == 0) {
            auto newNode = std::make_unique<Node>(value);
            newNode->next = std::move(head);
            head = std::move(newNode);
            ++count;
            return;
        }
    
        auto newNode = std::make_unique<Node>(value);
        std::unique_ptr<Node>* curr = &head;
    
        for (size_t i = 0; i < pos - 1; ++i) {
            curr = &((*curr)->next);
        }
    
        newNode->next = std::move((*curr)->next);
        (*curr)->next = std::move(newNode);
        ++count;
    }

    void erase(size_t pos) {
        if (pos >= count) {
            throw std::out_of_range("Invalid position");
        }
    
        std::unique_ptr<Node>* curr = &head;
    
        // Move curr to the unique_ptr that owns the node at position pos
        for (size_t i = 0; i < pos; ++i) {
            curr = &((*curr)->next);
        }
    
        // Bypass the node at pos
        *curr = std::move((*curr)->next);
    
        --count;
    }

    int find(const T& value) const {
        Node* curr = head.get();
        int pos = 0;

    while (curr) {
        if (curr->data == value) {
            return pos;
        } else {
            curr = curr->next.get();
            ++pos;  
        }
    }

    return -1;
    }


    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    const T& front() const { return head->data; }
    T& front() {return head->data;}

    const T& back() const {
        std::unique_ptr<Node>* curr = &head;
    
        // Traverse until we find a null unique_ptr
        while ((*curr)->next != nullptr) {
            curr = &((*curr)->next);
        }
        return (*curr)->data;
    }

    T& back() {
        std::unique_ptr<Node>* curr = &head;
    
        // Traverse until we find a null unique_ptr
        while ((*curr)->next != nullptr) {
            curr = &((*curr)->next);
        }
        return (*curr)->data;
    }

    void print() const {
        Node* curr = head.get();   // non-owning traversal pointer
    
        while (curr) {
            std::cout << curr->data << std::endl;
            curr = curr->next.get();
        }
    }

    // Iterator support (minimal)
    class iterator {
        Node* current;
    public:
        explicit iterator(Node* ptr = nullptr) : current(ptr) {}
        T& operator*() const { return current->data; }
        iterator& operator++();
        iterator operator++(int);
        bool operator!=(const iterator& other) const { return current != other.current; }
        friend class LinkedList;
    };

    iterator begin() { return iterator(head.get()); }
    iterator end() const { return iterator(head.get()); }
    iterator end()   { return iterator(nullptr); }
    const_iterator() const { return iterator(nullptr); }
};