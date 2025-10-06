#pragma once
#ifndef NODE_H
#define NODE_H

#include <memory>
#include <string>
#include <sstream>
#include <utility>

/*
 * Node<T, Id>
 *  - T   : payload type stored in the node
 *  - Id  : identifier type used as node key (default size_t)
 *
 * Minimal Node holding an id and a value.
 */

template <typename T, typename Id = std::size_t>
class Node {
public:
    using id_type = Id;
    using value_type = T;

    Node() = default;
    explicit Node(id_type id) : id_(id) {}
    Node(id_type id, const value_type& v) : id_(id), value_(v) {}
    Node(id_type id, value_type&& v) : id_(id), value_(std::move(v)) {}

    // Accessors
    id_type id() const noexcept { return id_; }
    const value_type& value() const noexcept { return value_; }
    value_type& value() noexcept { return value_; }

    // Mutators
    void set_value(const value_type& v) { value_ = v; }
    void set_value(value_type&& v) { value_ = std::move(v); }

    // convenience for debug / printing
    std::string to_string() const {
        std::ostringstream os;
        os << "Node(" << id_ << ")";
        return os.str();
    }

private:
    id_type id_ {};
    value_type value_ {};
};

#endif // NODE_H
