#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
/*
 *  we need a tree to represent the calculation
 */
template <typename T>
class Node
{
    T const value;

public:
    T get() const
    {
        return value;
    }
    Node(Node &) = delete;
    Node(T v) : value(v) {}
    virtual ~Node() {}
};
template <typename T>
class NodeWithChildren : public Node<T>
{
    std::shared_ptr<Node<T>> left, right;

public:
    using Node<T>::get;
    NodeWithChildren(T v, std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right) : Node<T>(v), left(left), right(right) {}
    std::shared_ptr<Node<T>> get_left() const
    {
        return left;
    }
    std::shared_ptr<Node<T>> get_right() const
    {
        return right;
    }
};
template <typename T>
using TreeList = std::vector<std::shared_ptr<Node<T>>>;
template <typename T>
std::shared_ptr<Node<T>> make_leaf(T v)
{
    return std::make_shared<Node<T>>(v);
}
template <typename T>
std::shared_ptr<Node<T>> make_tree(T v, std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right)
{
    return std::make_shared<NodeWithChildren<T>>(v, left, right);
}
// it is kinda like partition, the operation can make (`a`, `+`, `b`) from `+` and (`a`, `b`)
template <typename T>
TreeList<T> make_trees(T v, TreeList<T> lhs, TreeList<T> rhs)
{
    TreeList<T> ret;
    for (auto &i : lhs)
    {
        for (auto &j : rhs)
        {
            ret.push_back(make_tree(v, i, j));
        }
    }
    return ret;
}
// insert elements from a vector to a binary tree in order
template <typename T>
TreeList<T> make_possible_trees(std::vector<T> const &stack)
{
    auto size = stack.size();
    assert(size % 2 == 1);
    TreeList<T> ret;
    if (size == 1)
    {
        ret.push_back(make_leaf(stack[0]));
        return ret;
    }
    for (auto i = 1u; i < size; i += 2)
    {
        auto lefts = make_possible_trees(std::vector<T>(stack.begin(), stack.begin() + i));
        auto rights = make_possible_trees(std::vector<T>(stack.begin() + i + 1, stack.end()));
        auto trees = make_trees(stack[i], lefts, rights);
        ret.insert(ret.end(), trees.begin(), trees.end());
    }
    return ret;
}
// to print the entire tree
template <typename T>
std::ostream &operator<<(std::ostream &os, Node<T> const &n)
{
    if (auto with_children = dynamic_cast<NodeWithChildren<T> const *>(&n))
    {
        os << "(" << with_children->get_left() << " " << n.get() << " " << with_children->get_right() << ")";
    }
    else
    {
        os << n.get();
    }
    return os;
}
template <typename T>
std::ostream &operator<<(std::ostream &os, std::shared_ptr<T> const &p)
{
    return os << *p.get();
}
template <typename T>
std::ostream &operator<<(std::ostream &os, std::vector<T> const &v)
{
    os << "[";
    if (!v.empty())
        os << v[0];
    for (auto i = 1u; i < v.size(); ++i)
    {
        os << ", " << v[i];
    }
    return os << " ]";
}
/*
* Okay, enough code for manipulating the tree
*/
// need some basic elements to fill the tree, that's our `Item` virtual class
class Item
{
public:
    Item(Item &) = delete;
    Item() = default;
    virtual ~Item() {}
    virtual void print(std::ostream &) const = 0;
};
std::ostream &operator<<(std::ostream &os, Item const &i)
{
    i.print(os);
    return os;
}
class Operator : public Item
{
public:
    virtual double apply(double, double) const = 0;
};
class Plus : public Operator
{
public:
    void print(std::ostream &os) const
    {
        os << '+';
    }
    Plus() = default;
    double apply(double lhs, double rhs) const
    {
        return lhs + rhs;
    }
};
class Minus : public Operator
{
public:
    void print(std::ostream &os) const
    {
        os << '-';
    }
    Minus() = default;
    double apply(double lhs, double rhs) const
    {
        return lhs - rhs;
    }
};
class Multiple : public Operator
{
public:
    void print(std::ostream &os) const
    {
        os << '*';
    }
    Multiple() = default;
    double apply(double lhs, double rhs) const
    {
        return lhs * rhs;
    }
};
class Divide : public Operator
{
public:
    void print(std::ostream &os) const
    {
        os << '/';
    }
    Divide() = default;
    double apply(double lhs, double rhs) const
    {
        return lhs / rhs;
    }
};
class Number : public Item
{
    double const value;

public:
    void print(std::ostream &os) const
    {
        os << value;
    }
    Number(double v) : value(v) {}
    double get() const
    {
        return value;
    }
};
std::string unexpected(std::shared_ptr<Item> i)
{
    std::stringstream ss;
    ss << "unexpected token `";
    i->print(ss);
    ss << '`';
    return ss.str();
}
std::string unexpected(unsigned c)
{
    std::stringstream ss;
    ss << "unexpected character `" << c << '`';
    return ss.str();
}
// do the maths
double calc(std::shared_ptr<Node<std::shared_ptr<Item>>> const &n)
{
    if (auto with_children = dynamic_cast<NodeWithChildren<std::shared_ptr<Item>> const *>(n.get()))
    {
        auto left = calc(with_children->get_left());
        if (auto op = dynamic_cast<Operator const *>(n->get().get()))
        {
            auto right = calc(with_children->get_right());
            return op->apply(left, right);
        }
        throw std::string("internal error");
    }
    else
    {
        auto number = dynamic_cast<Number const *>(n->get().get());
        if (number)
        {
            return number->get();
        }
        throw std::string("internal error");
    }
}
// to parse
class GetNumber
{
    bool _empty;
    double _value;

public:
    GetNumber() : _empty(true) {}
    void push(char c)
    {
        if (c >= '0' && c <= '9')
        {
            if (_empty)
            {
                _value = c - '0';
                _empty = false;
            }
            else
            {
                if (_value == .0)
                    throw unexpected(c);
                _value = c - '0' + _value * 10.;
            }
        }
        else
        {
            throw unexpected(c);
        }
    }
    bool empty() { return _empty; }
    std::shared_ptr<Number> pop()
    {
        if (_empty)
            throw std::exception();
        _empty = true;
        return std::make_shared<Number>(_value);
    }
};
// read bytes to get a `vector<shared_ptr<Item>>`
std::vector<std::shared_ptr<Item>> read_line(std::istream &in)
{
    std::vector<std::shared_ptr<Item>> ret;
    GetNumber get_number;
    auto pop_number = [&]() {
        if (!get_number.empty())
        {
            ret.push_back(get_number.pop());
        }
    };
    for (;;)
    {
        char c;
        in.read(&c, 1);
        if (c >= '0' && c <= '9')
        {
            try
            {
                get_number.push(c);
            }
            catch (std::string &ex)
            {
                while (c != '\n')
                    in.read(&c, 1);
                throw ex;
            }
        }
        else
        {
            try
            {
                switch (c)
                {
                case '\n':
                    pop_number();
                    return ret;
                case ' ':
                case '\r':
                case '\t':
                    pop_number();
                    break;
                case '+':
                    pop_number();
                    ret.push_back(std::make_shared<Plus>());
                    break;
                case '-':
                    pop_number();
                    ret.push_back(std::make_shared<Minus>());
                    break;
                case '*':
                    pop_number();
                    ret.push_back(std::make_shared<Multiple>());
                    break;
                case '/':
                    pop_number();
                    ret.push_back(std::make_shared<Divide>());
                    break;
                default:
                    throw std::exception();
                }
            }
            catch (std::exception &)
            {
                while (c != '\n')
                    in.read(&c, 1);
                throw unexpected(c);
            }
        }
    }
}
// make sure operators and numbers are in the right place
std::vector<std::shared_ptr<Item>>
first_validation(std::vector<std::shared_ptr<Item>> from)
{
    if (from.empty())
        throw std::string("empty");
    for (auto i = 0u; i < from.size(); ++i)
    {
        auto item = from[i].get();
        if (i % 2u)
        {
            if (dynamic_cast<Plus *>(item) || dynamic_cast<Minus *>(item) ||
                dynamic_cast<Multiple *>(item) || dynamic_cast<Divide *>(item))
            {
                if (i + 1u == from.size())
                {
                    throw unexpected(from[i]);
                }
            }
            else
            {
                throw unexpected(from[i]);
            }
        }
        else
        {
            if (!dynamic_cast<Number *>(item))
            {
                throw unexpected(from[i]);
            }
        }
    }
    return from;
}
// we count a weight for each tree, to make sure we don't get `42-(1-2)` for `42-1-2`
int count(std::shared_ptr<Node<std::shared_ptr<Item>>> tree)
{
    if (auto ptr = dynamic_cast<NodeWithChildren<std::shared_ptr<Item>> *>(tree.get()))
    {
        return 2 * count(ptr->get_left()) + count(ptr->get_right());
    }
    return 1;
}
// make sure we do multiplication and division first
bool has_plus_or_minus(std::shared_ptr<Node<std::shared_ptr<Item>>> tree)
{
    auto ptr = tree.get()->get();
    if (dynamic_cast<Plus *>(ptr.get()) || dynamic_cast<Minus *>(ptr.get()))
    {
        return true;
    }
    if (auto children = dynamic_cast<NodeWithChildren<std::shared_ptr<Item>> *>(tree.get()))
    {
        return has_plus_or_minus(children->get_left()) || has_plus_or_minus(children->get_right());
    }
    return false;
}
// make sure we do multiplication and division first
bool validate(std::shared_ptr<Node<std::shared_ptr<Item>>> tree)
{
    if (auto with_children = dynamic_cast<NodeWithChildren<std::shared_ptr<Item>> *>(tree.get()))
    {
        auto value = tree->get().get();
        if (dynamic_cast<Multiple *>(value) || dynamic_cast<Divide *>(value))
        {
            return !has_plus_or_minus(with_children->get_left()) && !has_plus_or_minus(with_children->get_right());
        }
        else
        {
            return validate(with_children->get_left()) && validate(with_children->get_right());
        }
    }
    return true;
}
/*
 *  `42-1-1` means `(42-1)-1` but not `42-(1-1)`,
 *  and `1+2*3` means `1+(2*3)`, not `(1+2)*3`,
 *  so we use `count()` and `validate()` to filter out incorrect result
 */
TreeList<std::shared_ptr<Item>> filter(TreeList<std::shared_ptr<Item>> from)
{
    int max = -1;
    std::vector<int> list;
    for (auto i = 0u; i < from.size(); ++i)
    {
        if (validate(from[i]))
        {
            auto cnt = count(from[i]);
            list.push_back(cnt);
            if (cnt > max)
                max = cnt;
        }
        else
        {
            list.push_back(-1);
        }
    }
    TreeList<std::shared_ptr<Item>> ret;
    for (auto i = 0u; i < from.size(); ++i)
    {
        if (list[i] == max)
        {
            ret.push_back(from[i]);
        }
    }
    return ret;
}
int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (std::string("-h") == argv[1] || std::string("--help") == argv[1])
        {
            std::cout << "try `" << argv[0] << " 1+2` or something" << std::endl;
            return 0;
        }
        std::stringstream ss;
        ss << argv[1];
        ss << '\n';
        try
        {
            auto items = first_validation(read_line(ss));
            for (auto tree : filter(make_possible_trees(items)))
            {
                std::cout << tree << " = " << calc(tree) << std::endl;
            }
        }
        catch (std::string &ex)
        {
            std::cout << "error: " << ex << std::endl;
        }
        return 0;
    }
    for (;;)
    {
        try
        {
            auto items = first_validation(read_line(std::cin));
            for (auto tree : filter(make_possible_trees(items)))
            {
                std::cout << tree << " = " << calc(tree) << std::endl;
            }
        }
        catch (std::string &ex)
        {
            std::cout << "error: " << ex << std::endl;
        }
    }
    return 0;
}