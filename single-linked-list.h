#pragma once

#include <cassert>
#include <cstddef>
#include <utility>
#include <initializer_list>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

public:

    explicit SingleLinkedList()
        : head_()
        , size_(0)
    {
    }

    ~SingleLinkedList() {
        Clear();
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PushBack(const Type& value) {
        Node* new_value = new Node(value, nullptr);
        Node* last_item_ptr = &head_;
        for (size_t i = 0; i < size_; ++i) {
            last_item_ptr = last_item_ptr->next_node;
        }
        last_item_ptr->next_node = new_value;
        ++size_;
    }

    void PopFront() noexcept {
        if (size_ != 0) {
            Node* tmp_ptr = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete tmp_ptr;
            --size_;
        }
    }

    void Clear() noexcept {
        while (size_ != 0) {
            this->PopFront();
        }
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return !static_cast<bool>(size_);
    }

    template <typename It>
    void CopyFromRange(It begin, It end) {
        Iterator insert_after_ptr = this->before_begin();
        for (It it = begin; it != end; ++it) {
            this->InsertAfter(insert_after_ptr, *it);
            ++insert_after_ptr;
        }
    }

    SingleLinkedList(std::initializer_list<Type> values)
        : head_()
        , size_(0) {
        SingleLinkedList copy;
        copy.CopyFromRange(values.begin(), values.end());
        this->swap(copy);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        if (this != &other) {
            SingleLinkedList copy;
            copy.CopyFromRange(other.begin(), other.end());
            this->swap(copy);
        }
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList copy(rhs);
            this->swap(copy);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    template <typename ValueType>
    class BasicIterator {
        template <typename T>
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node)
            :node_(node) {
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return (this->node_ == rhs.node_);
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator tmp_ptr(node_);
            ++(*this);
            return tmp_ptr;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

        Node* GetNodePtr() {
            return node_;
        }

    private:
        Node* node_ = nullptr;
    };

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        Iterator output(&head_);
        return output;
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        ConstIterator output(const_cast<Node*>(&head_));
        return output;
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        ConstIterator output(const_cast<Node*>(&head_));
        return output;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.GetNodePtr() != nullptr);
        Node* tmp_ptr = pos.GetNodePtr()->next_node;
        Node* new_item = new Node(value, tmp_ptr);
        pos.GetNodePtr()->next_node = new_item;
        ++size_;
        return Iterator(new_item);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.GetNodePtr() != nullptr);
        if (pos.GetNodePtr()->next_node == nullptr) {
            return Iterator(nullptr);
        }
        Node* ptr_after_deleted = pos.GetNodePtr()->next_node->next_node;
        delete pos.GetNodePtr()->next_node;
        pos.GetNodePtr()->next_node = ptr_after_deleted;
        --size_;
        Iterator output(ptr_after_deleted);
        return output;
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() == rhs.GetSize()) {
        auto lhs_it = lhs.begin();
        auto rhs_it = rhs.begin();
        for (size_t i = 0; i < lhs.GetSize(); ++i) {
            if (*lhs_it != *rhs_it) {
                return false;
            }
            ++lhs_it;
            ++rhs_it;
        }
        return true;
    }
    return false;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
