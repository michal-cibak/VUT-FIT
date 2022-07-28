// Project: ICP - Editor a interpret hierarchicky strukturovaných funkčních bloků
// Authors: Michal Cibák - xcibak00, FIT VUT
//          Tomáš Hrúz   - xhruzt00, FIT VUT
// Date: 15.4.2021


#ifndef GUARD_FUNBLOCKS_HPP
#define GUARD_FUNBLOCKS_HPP


#include <string>
#include <vector>
#include <list>
#include <map>


class Port;
class Input;
class Output;
class Block;
class Atomic;
class Composite;
class Library;


class Port
{
    friend Block; // for parent and name inherited in Input and Output
protected:
    Block *parent;
    std::string name;
    Port(Block *p, std::string n);
    Port(const Port& port) = default;
public:
    ~Port() = default;
    bool is_connected();
};

class Input : public Port
{
    friend Output; // for connection
    friend Block; // for constructors
    Output *connection;
    Input(Block *p, std::string n = "", std::string v = "");
    // Input(const Input& input);
public:
    std::string value;
    ~Input() = default;
    bool is_connected();
    void connect(Output *output);
};

class Output : public Port
{
    friend Input; // for connections
    friend Block; // for constructors
    std::vector<Input *> connections;
    Output(Block *p, std::string n = "");
    // Output(const Output& output);
public:
    ~Output() = default;
    bool is_connected();
    void connect(Input *input);
};

class Block
{
    friend Atomic;
    friend Composite;
    friend Input; // to access blocks parent when connecting ports
    friend Output; // to access blocks parent when connecting ports
protected:
    Block *parent = nullptr;
    Block(std::string n);
    Block(std::string n, int in, int out);
    Block(const Block& block); // disconnects the block if it was connected
public:
    std::string name;
    std::list<Input> inputs;
    std::list<Output> outputs;
    ~Block() = default;
    bool is_connected();
    Input * get_input(unsigned which);
    Input * get_input(std::string which);
    Output * get_output(unsigned which = 0);
    Output * get_output(std::string which);
    // void rename_input(unsigned which, std::string name);
    // void rename_input(std::string which, std::string name);
    // void rename_output(unsigned which, std::string name);
    // void rename_output(std::string which, std::string name);
    void add_input(std::string name = "", std::string value = "");
    void add_output(std::string name = "");
    // void remove_input(unsigned which);
    // void remove_input(std::string name);
};

class Atomic : public Block
{
    std::string code;
public:
    Atomic(std::string n);
    Atomic(std::string n, int in, int out, std::string c = "");
    Atomic(const Atomic&) = default;
    ~Atomic() = default;
};

class Composite : public Block
{
    // friend Library;
    static int instance_number;
    std::vector<Block *> blocks;
public:
    Composite();
    Composite(std::string n);
    Composite(int in, int out);
    Composite(std::string n, int in, int out);
    Composite(const Composite&) = default; // TODO - ++instance_number, blocks...
    ~Composite() = default;
    void add_block(Block *block);
    // void add_blocks(Block *block...);

    struct GenerationError
    {
        std::string error;
        GenerationError() = default;
        GenerationError(std::string cause) : error{cause} {}
        ~GenerationError() = default;
    };
    void generate();
};

class Library
{
public:
    std::map<std::string, Atomic> atomic_blocks;
    std::map<std::string, Composite> composite_blocks;
    std::map<std::string, Composite> applications;
    Library();
    ~Library();
};


#endif // GUARD_FUNBLOCKS_HPP
