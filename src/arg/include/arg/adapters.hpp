#pragma once

#include "arg/arguments.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace arg {

template <class T>
bool read(std::string_view input, T& value)
{
    auto stream = std::istringstream{std::string{input}};
    stream >> value;
    return !!stream;
}

inline bool read(std::string_view input, std::string& string)
{
    auto stream = std::istringstream{std::string{input}};
    // This is probably inefficient.
    string = {std::istreambuf_iterator<char>(stream), {}};
    return true;
}

class KeyAdapter {
public:
    virtual ~KeyAdapter() = default;

    [[nodiscard]] virtual bool hasArgument() const = 0;
    [[nodiscard]] virtual bool isRequired() const = 0;
    [[nodiscard]] virtual bool isSet() const = 0;
    [[nodiscard]] virtual const std::vector<std::string>& keys() const = 0;
    [[nodiscard]] virtual std::string metavar() const = 0;
    [[nodiscard]] virtual const std::string& help() const = 0;
    [[nodiscard]] virtual bool multi() const = 0;

    virtual void raise() = 0;
    virtual bool addValue(std::string_view) = 0;

    [[nodiscard]] std::string firstKey() const
    {
        return keys().empty() ? "<no key>" : keys().front();
    }

    [[nodiscard]] std::string keyString() const
    {
        std::ostringstream stream;
        if (auto it = keys().begin(); it != keys().end()) {
            stream << *it++;
            for (; it != keys().end(); ++it) {
                stream << ", " << *it;
            }
        }
        return stream.str();
    }

    [[nodiscard]] bool hasKey(std::string_view s) const
    {
        return std::find(keys().begin(), keys().end(), s) != keys().end();
    }
};

class ArgumentAdapter {
public:
    virtual ~ArgumentAdapter() = default;

    [[nodiscard]] virtual bool isRequired() const = 0;
    [[nodiscard]] virtual bool isSet() const = 0;
    [[nodiscard]] virtual std::string metavar() const = 0;
    [[nodiscard]] virtual const std::string& help() const = 0;
    [[nodiscard]] virtual bool multi() const = 0;
    virtual bool addValue(std::string_view) = 0;
};

class FlagAdapter : public KeyAdapter {
public:
    explicit FlagAdapter(Flag&& flag)
        : _flag(std::move(flag))
    { }

    [[nodiscard]] bool hasArgument() const override
    {
        return false;
    }

    [[nodiscard]] bool isRequired() const override
    {
        return false;
    }

    [[nodiscard]] bool isSet() const override
    {
        throw std::logic_error{"FlagAdapter's isSet must not be called"};
    }

    void raise() override
    {
        _flag = true;
    }

    bool addValue(std::string_view) override
    {
        throw std::logic_error{"FlagAdapter's addValue must not be called"};
    }

    [[nodiscard]] const std::vector<std::string>& keys() const override
    {
        return _flag.keys();
    }

    [[nodiscard]] std::string metavar() const override
    {
        return "";
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _flag.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return false;
    }

private:
    Flag _flag;
};

class MultiFlagAdapter : public KeyAdapter {
public:
    explicit MultiFlagAdapter(MultiFlag multiFlag)
        : _multiFlag(std::move(multiFlag))
    { }

    [[nodiscard]] bool hasArgument() const override
    {
        return false;
    }

    [[nodiscard]] bool isRequired() const override
    {
        return false;
    }

    [[nodiscard]] bool isSet() const override
    {
        throw std::logic_error{"MultiFlagAdapter's isSet must not be called"};
    }

    void raise() override
    {
        _multiFlag = true;
    }

    bool addValue(std::string_view) override
    {
        throw std::logic_error{"MultiFlagAdapter's addValue must not be called"};
    }

    [[nodiscard]] const std::vector<std::string>& keys() const override
    {
        return _multiFlag.keys();
    }

    [[nodiscard]] std::string metavar() const override
    {
        return "";
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _multiFlag.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return true;
    }

private:
    MultiFlag _multiFlag;
};

template <class T>
class OptionAdapter : public KeyAdapter {
public:
    explicit OptionAdapter(Option<T>&& option)
        : _option(std::move(option))
    { }

    [[nodiscard]] bool hasArgument() const override
    {
        return true;
    }

    [[nodiscard]] bool isRequired() const override
    {
        return _option.isRequired();
    }

    [[nodiscard]] bool isSet() const override
    {
        return _option.isSet();
    }

    void raise() override
    {
        throw std::logic_error{"OptionAdapter's raise must not be called"};
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _option = std::move(value);
            return true;
        }
        return false;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const override
    {
        return _option.keys();
    }

    [[nodiscard]] std::string metavar() const override
    {
        return _option.metavar();
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _option.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return false;
    }

private:
    Option<T> _option;
};

template <class T>
class MultiOptionAdapter : public KeyAdapter {
public:
    explicit MultiOptionAdapter(MultiOption<T>&& multiOption)
        : _multiOption(std::move(multiOption))
    { }

    [[nodiscard]] bool hasArgument() const override
    {
        return true;
    }

    [[nodiscard]] bool isRequired() const override
    {
        return false;
    }

    [[nodiscard]] bool isSet() const override
    {
        throw std::logic_error{"MultiOptionAdapter's isSet must not be called"};
    }

    void raise() override
    {
        throw std::logic_error{"MultiOptionAdapter's raise must not be called"};
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _multiOption.push(std::move(value));
            return true;
        }
        return false;
    }

    [[nodiscard]] const std::vector<std::string>& keys() const override
    {
        return _multiOption.keys();
    }

    [[nodiscard]] std::string metavar() const override
    {
        return _multiOption.metavar();
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _multiOption.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return true;
    }

private:
    MultiOption<T> _multiOption;
};

template <class T>
class ValueAdapter : public ArgumentAdapter {
public:
    explicit ValueAdapter(Value<T>&& value)
        : _value(std::move(value))
    { }

    [[nodiscard]] bool isRequired() const override
    {
        return _value.isRequired();
    }

    [[nodiscard]] bool isSet() const override
    {
        return _value.isSet();
    }

    [[nodiscard]] std::string metavar() const override
    {
        return _value.metavar();
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _value.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return false;
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _value = std::move(value);
            return true;
        }
        return false;
    }

private:
    Value<T> _value;
};

template <class T>
class MultiValueAdapter : public ArgumentAdapter {
    explicit MultiValueAdapter(MultiValue<T>&& multiValue)
        : _multiValue(std::move(multiValue))
    { }

    [[nodiscard]] bool isRequired() const override
    {
        return _multiValue.isRequired();
    }

    [[nodiscard]] bool isSet() const override
    {
        throw std::logic_error{"MultiValueAdapter's isSet must not be called"};
    }

    [[nodiscard]] std::string metavar() const override
    {
        return _multiValue.metavar();
    }

    [[nodiscard]] const std::string& help() const override
    {
        return _multiValue.help();
    }

    [[nodiscard]] bool multi() const override
    {
        return true;
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _multiValue.push(std::move(value));
            return true;
        }
        return false;
    }

private:
    MultiValue<T> _multiValue;
};

} // namespace arg
