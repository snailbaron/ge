#pragma once

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace arg {

class Flag {
public:
    template <class... Args>
    Flag keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    Flag help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    bool operator*() const
    {
        return _data->value;
    }

    bool& operator*()
    {
        return _data->value;
    }

    operator bool() const
    {
        return **this;
    }

    Flag& operator=(bool value)
    {
        _data->value = value;
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        bool value = false;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

inline std::ostream& operator<<(std::ostream& output, const Flag& flag)
{
    return output << *flag;
}

inline std::istream& operator>>(std::istream& input, Flag& flag)
{
    return input >> *flag;
}

class MultiFlag {
public:
    template <class... Args>
    MultiFlag keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    MultiFlag help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    size_t operator*() const
    {
        return _data->count;
    }

    size_t& operator*()
    {
        return _data->count;
    }

    operator size_t() const
    {
        return **this;
    }

    MultiFlag& operator=(size_t count)
    {
        _data->count = count;
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        size_t count = 0;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

inline std::ostream& operator<<(std::ostream& output, const MultiFlag& multiFlag)
{
    return output << *multiFlag;
}

inline std::istream& operator>>(std::istream& input, MultiFlag& multiFlag)
{
    return input >> *multiFlag;
}

template <class T>
class Option {
public:
    template <class... Args>
    Option keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    Option help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    Option metavar(std::string_view s)
    {
        _data->metavar = s;
        return *this;
    }

    [[nodiscard]] const std::string& metavar() const
    {
        return _data->metavar;
    }

    Option markRequired()
    {
        _data->required = true;
        return *this;
    }

    [[nodiscard]] bool isRequired() const
    {
        return _data->required;
    }

    Option defaultValue(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

    [[nodiscard]] bool isSet() const
    {
        return _data->isSet;
    }

    const T& operator*() const
    {
        return _data->value;
    }

    T& operator*()
    {
        return _data->value;
    }

    const T* operator->() const
    {
        return &_data->value;
    }

    T* operator->()
    {
        return &_data->value;
    }

    operator const T&() const
    {
        return **this;
    }

    Option& operator=(T&& value)
    {
        _data->value = std::forward<T>(value);
        _data->isSet = true;
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        std::string metavar = "VALUE";
        bool required = false;
        T value = T{};
        bool isSet = false;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
std::ostream& operator<<(std::ostream& output, const Option<T>& option)
{
    return output << *option;
}

template <class T>
std::istream& operator>>(std::istream& input, Option<T>& option)
{
    return input >> *option;
}

template <class T>
class MultiOption {
public:
    template <class... Args>
    MultiOption keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    MultiOption help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    MultiOption metavar(std::string_view s)
    {
        _data->metavar = s;
        return *this;
    }

    [[nodiscard]] const std::string& metavar() const
    {
        return _data->metavar;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    auto begin() const
    {
        return _data->value.begin();
    }

    auto begin()
    {
        return _data->value.begin();
    }

    auto end() const
    {
        return _data->value.end();
    }

    auto end()
    {
        return _data->value.end();
    }

    void push(T&& value)
    {
        _data->values.push_back(std::forward<T>(value));
    }

    const std::vector<T>& vector() const
    {
        return _data->values;
    }

    std::vector<T>& vector()
    {
        return _data->values;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        std::string metavar = "VALUE";
        std::vector<T> values;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
std::ostream& operator<<(std::ostream& output, const MultiOption<T>& multiOption)
{
    return output << *multiOption;
}

template <class T>
std::istream& operator>>(std::istream& input, MultiOption<T>& multiOption)
{
    return input >> *multiOption;
}

template <class T>
class Value {
public:
    Value help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    Value metavar(std::string_view s)
    {
        _data->metavar = s;
        return *this;
    }

    [[nodiscard]] const std::string& metavar() const
    {
        return _data->metavar;
    }

    Value markRequired()
    {
        _data->required = true;
        return *this;
    }

    [[nodiscard]] bool isRequired() const
    {
        return _data->required;
    }

    Value defaultValue(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

    [[nodiscard]] bool isSet() const
    {
        return _data->isSet;
    }

    const T& operator*() const
    {
        return _data->value;
    }

    T& operator*()
    {
        return _data->value;
    }

    const T* operator->() const
    {
        return &_data->value;
    }

    T* operator->()
    {
        return &_data->value;
    }

    operator const T&() const
    {
        return **this;
    }

    Value& operator=(T&& value)
    {
        _data->value = std::forward<T>(value);
        _data->isSet = true;
        return *this;
    }

private:
    struct Data {
        std::string help;
        std::string metavar = "VALUE";
        bool required = false;
        T value = T{};
        bool isSet = false;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
std::ostream& operator<<(std::ostream& output, const Value<T>& value)
{
    return output << *value;
}

template <class T>
std::istream& operator>>(std::istream& input, Value<T>& value)
{
    return input >> *value;
}

template <class T>
class MultiValue {
public:
    MultiValue help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    [[nodiscard]] const std::string& help() const
    {
        return _data->help;
    }

    auto begin() const
    {
        return _data->value.begin();
    }

    auto begin()
    {
        return _data->value.begin();
    }

    auto end() const
    {
        return _data->value.end();
    }

    auto end()
    {
        return _data->value.end();
    }

    void push(T&& value)
    {
        _data->values.push_back(std::forward<T>(value));
    }

    const std::vector<T>& vector() const
    {
        return _data->values;
    }

    std::vector<T>& vector()
    {
        return _data->values;
    }

private:
    struct Data {
        std::string help;
        std::string metavar = "VALUE";
        std::vector<T> values;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
std::ostream& operator<<(std::ostream& output, const MultiValue<T>& multiValue)
{
    return output << *multiValue;
}

template <class T>
std::istream& operator>>(std::istream& input, MultiValue<T>& multiValue)
{
    return input >> *multiValue;
}

} // namespace arg
