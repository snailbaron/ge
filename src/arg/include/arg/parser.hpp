#pragma once

#include "arg/adapters.hpp"
#include "arg/arguments.hpp"
#include "arg/errors.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace arg {

class Parser {
public:
    struct Config {
        bool allowKeyValueSyntax = true;
        std::string keyValueSeparator = "=";
        bool allowArgumentPacking = true;
        std::string packPrefix = "-";
        bool allowUnspecifiedArguments = false;
    };

    void attach(Flag flag)
    {
        _options.push_back(std::make_unique<FlagAdapter>(std::move(flag)));
    }

    void attach(MultiFlag multiFlag)
    {
        _options.push_back(
            std::make_unique<MultiFlagAdapter>(std::move(multiFlag)));
    }

    template <class T>
    void attach(Option<T> option)
    {
        _options.push_back(
            std::make_unique<OptionAdapter<T>>(std::move(option)));
    }

    template <class T>
    void attach(MultiOption<T> multiOption)
    {
        _options.push_back(
            std::make_unique<MultiOptionAdapter<T>>(std::move(multiOption)));
    }

    template <class T>
    void attach(Value<T> value)
    {
        _arguments.push_back(std::make_unique<ValueAdapter<T>>(std::move(value)));
    }

    template <class T>
    void attach(MultiValue<T> multiValue)
    {
        _arguments.push_back(
            std::make_unique<MultiValueAdapter<T>>(std::move(multiValue)));
    }

    Flag flag()
    {
        return makeAndAttach<Flag>();
    }

    MultiFlag multiFlag()
    {
        return makeAndAttach<MultiFlag>();
    }

    template <class T>
    Option<T> option()
    {
        return makeAndAttach<Option<T>>();
    }

    template <class T>
    MultiOption<T> multiOption()
    {
        return makeAndAttach<MultiOption<T>>();
    }

    template <class T>
    Value<T> argument()
    {
        return makeAndAttach<Value<T>>();
    }

    template <class T>
    MultiValue<T> multiArgument()
    {
        return makeAndAttach<MultiValue<T>>();
    }

    template <class... Args>
    void helpKeys(Args&&... args)
    {
        _helpKeys = {std::forward<Args>(args)...};
    }

    void printHelp(std::ostream& output=std::cout) const
    {
        output << "usage: " << _programName;
        for (const auto& option : _options) {
            if (option->multi()) {
                output << " {";
            } else if (!option->isRequired()) {
                output << " [";
            }
            output << " " << option->firstKey();
            if (option->hasArgument()) {
                output << " " << option->metavar();
            }
            if (option->multi()) {
                output << " }";
            } else if (!option->isRequired()) {
                output << " ]";
            }
        }
        for (const auto& argument : _arguments) {
            if (argument->multi()) {
                output << " {";
            } else if (!argument->isRequired()) {
                output << " [";
            }
            output << " " << argument->metavar();
            if (argument->multi()) {
                output << " }";
            } else if (!argument->isRequired()) {
                output << " ]";
            }
        }
        output << "\n";

        if (!_options.empty()) {
            output << "\nOptions:\n";
            for (const auto& option : _options) {
                output << "  " << option->keyString();
                if (option->hasArgument()) {
                    output << " " << option->metavar();
                }
                output << "  " << option->help() << "\n";
            }
        }

        if (!_arguments.empty()) {
            output << "\nPositional arguments:\n";
            for (const auto& argument : _arguments) {
                output << argument->metavar() << "  " <<
                    argument->help() << "\n";
            }
        }
    }

    void parse(int argc, char** argv)
    {
        if (argc > 0) {
            _programName = std::filesystem::path{argv[0]}.filename().string();
        }

        std::vector<std::string> args;
        for (int i = 1; i < argc; i++) {
            args.emplace_back(argv[i]);
        }
        parse(args);
    }

    void parse(const std::vector<std::string>& args)
    {
        std::vector<err::Error> errors;
        bool helpRequested = false;

        for (auto arg = args.begin(); arg != args.end(); ) {
            if (auto it = std::find(_helpKeys.begin(), _helpKeys.end(), *arg);
                    it != _helpKeys.end()) {
                helpRequested = true;
                ++arg;
                continue;
            }

            if (auto* option = findOption(*arg); option) {
                if (option->hasArgument()) {
                    auto givenKey = *arg;
                    ++arg;
                    if (arg == args.end()) {
                        errors.emplace_back(
                            err::RequiredOptionValueNotGiven{givenKey});
                        continue;
                    }
                    if (option->addValue(*arg)) {
                        ++arg;
                    } else {
                        errors.emplace_back(
                            err::InvalidValueGiven{option->keyString(), *arg});
                    }
                } else {
                    option->raise();
                    ++arg;
                }
                continue;
            }

            if (auto pair = parseKeyValue(*arg); pair) {
                if (auto* option = findOption(pair->key); option) {
                    if (option->hasArgument()) {
                        option->addValue(pair->value);
                    } else {
                        errors.emplace_back(err::UnexpectedOptionValueGiven{
                                pair->key, pair->value});
                    }
                    ++arg;
                    continue;
                }
            }

            if (auto pack = parsePack(*arg); pack) {
                assert(!pack->keys.empty());

                for (size_t i = 0; i + 1 < pack->keys.size(); i++) {
                    auto* option = findOption(pack->keys.at(i));
                    assert(option);
                    assert(!option->hasArgument());
                    option->raise();
                }

                auto* lastOption = findOption(pack->keys.back());
                if (lastOption->hasArgument()) {
                    if (!pack->leftover.empty()) {
                        if (!lastOption->addValue(pack->leftover)) {
                            errors.emplace_back(err::InvalidValueGiven{
                                pack->keys.back(), pack->leftover});
                        }
                        ++arg;
                    } else {
                        ++arg;
                        if (arg == args.end()) {
                            errors.emplace_back(
                                err::RequiredOptionValueNotGiven{
                                    pack->keys.back()});
                            continue;
                        }
                        if (lastOption->addValue(*arg)) {
                            ++arg;
                        } else {
                            errors.emplace_back(err::InvalidValueGiven{
                                    pack->keys.back(), *arg});
                        }
                    }
                } else {
                    lastOption->raise();
                    ++arg;
                }
                continue;
            }

            if (_position < _arguments.size()) {
                auto* argument = _arguments.at(_position).get();
                if (!argument->addValue(*arg)) {
                    errors.emplace_back(
                        err::InvalidValueGiven{argument->metavar(), *arg});
                }
                ++arg;
                if (!argument->multi()) {
                    _position++;
                }
                continue;
            }

            if (config.allowUnspecifiedArguments) {
                _leftovers.push_back(*arg);
            } else {
                errors.emplace_back(err::UnexpectedArgument{*arg});
            }
            ++arg;
        }

        if (!helpRequested) {
            for (const auto& option : _options) {
                if (option->isRequired() && !option->isSet()) {
                    errors.emplace_back(
                        err::RequiredOptionNotSet{option->keyString()});
                }
            }
            for (const auto& argument : _arguments) {
                if (argument->isRequired() && !argument->isSet()) {
                    errors.emplace_back(
                        err::RequiredOptionNotSet{argument->metavar()});
                }
            }
        }

        if (!errors.empty()) {
            for (const auto& error : errors) {
                print(std::cerr, error);
            }
            printHelp(std::cerr);
            std::exit(EXIT_FAILURE);
        }

        if (helpRequested) {
            printHelp(std::cout);
            std::exit(EXIT_SUCCESS);
        }
    }

    Config config;

private:
    struct KeyValuePair {
        std::string key;
        std::string value;
    };

    struct KeyPack {
        std::vector<std::string> keys;
        std::string leftover;
    };

    template <class T>
    T makeAndAttach()
    {
        T arg;
        attach(arg);
        return arg;
    }

    // NOTE: linear search here, probably should replace with something better
    // someday
    KeyAdapter* findOption(std::string_view key)
    {
        for (auto it = _options.rbegin(); it != _options.rend(); ++it) {
            if ((*it)->hasKey(key)) {
                return it->get();
            }
        }
        return nullptr;
    }

    [[nodiscard]]
    std::optional<KeyValuePair> parseKeyValue(std::string_view arg) const
    {
        if (!config.allowKeyValueSyntax) {
            return std::nullopt;
        }

        auto sep = arg.find(config.keyValueSeparator);
        if (sep == std::string_view::npos) {
            return std::nullopt;
        }

        KeyValuePair keyValue;
        keyValue.key = arg.substr(0, sep);
        if (sep + 1 < arg.size()) {
            keyValue.value = arg.substr(sep + 1);
        }
        return keyValue;
    }

    std::optional<KeyPack> parsePack(std::string_view arg)
    {
        if (!config.allowArgumentPacking) {
            return std::nullopt;
        }

        if (arg.find(config.packPrefix) != 0) {
            return std::nullopt;
        }

        KeyPack keyPack;
        size_t i = config.packPrefix.length();
        for (; i < arg.length(); i++) {
            auto key = config.packPrefix + arg.at(i);
            auto* option = findOption(key);
            if (!option) {
                return std::nullopt;
            }

            keyPack.keys.push_back(key);
            if (option->hasArgument() && i + 1 < arg.length()) {
                keyPack.leftover = arg.substr(i + 1);
                return keyPack;
            }
        }
        return keyPack;
    }

    std::vector<std::unique_ptr<KeyAdapter>> _options;
    std::vector<std::unique_ptr<ArgumentAdapter>> _arguments;
    size_t _position = 0;
    std::vector<std::string> _leftovers;
    std::string _programName = "<program>";
    std::vector<std::string> _helpKeys;
};

namespace internal {

inline Parser globalParser;

} // namespace internal

inline Flag flag()
{
    return internal::globalParser.flag();
}

inline MultiFlag multiFlag()
{
    return internal::globalParser.multiFlag();
}

template <class T>
Option<T> option()
{
    return internal::globalParser.option<T>();
}

template <class T>
MultiOption<T> multiOption()
{
    return internal::globalParser.multiOption<T>();
}

template <class T>
Value<T> argument()
{
    return internal::globalParser.argument<T>();
}

template <class T>
MultiValue<T> multiArgument()
{
    return internal::globalParser.multiArgument<T>();
}

template <class... Args>
void helpKeys(Args&&... args)
{
    internal::globalParser.helpKeys(std::forward<Args>(args)...);
}

inline void parse(int argc, char** argv)
{
    return internal::globalParser.parse(argc, argv);
}

} // namespace arg
