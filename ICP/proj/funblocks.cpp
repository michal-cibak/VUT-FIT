// Project: ICP - Editor a interpret hierarchicky strukturovaných funkčních bloků
// Authors: Michal Cibák - xcibak00, FIT VUT
//          Tomáš Hrúz   - xhruzt00, FIT VUT
// Date: 15.4.2021


#include "funblocks.hpp"

#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>


Port::Port(Block *p, std::string n) : parent{p}, name{n} {}
bool Port::is_connected()
{
    return false;
}


Input::Input(Block *p, std::string n, std::string v) : Port{p, n}, connection{nullptr}, value{v} {}
bool Input::is_connected()
{
    if (connection)
        return true;
    else
        return false;
}
void Input::connect(Output *output)
{
    if (!connection && output->parent->parent == this->parent->parent)
    {
        connection = output;
        output->connections.push_back(this);
    }
}


Output::Output(Block *p, std::string n) : Port{p, n} {}
bool Output::is_connected()
{
    if (connections.empty())
        return false;
    else
        return true;
}
void Output::connect(Input *input)
{
    if (!input->connection && input->parent->parent == this->parent->parent)
    {
        connections.push_back(input);
        input->connection = this;
    }
}


Block::Block(std::string n) : name{n} {}
Block::Block(std::string n, int in, int out) : name{n}
{
    for (int i = 0; i < in; i++)
        inputs.push_back(std::move(Input(this)));
    for (int o = 0; o < out; o++)
        outputs.push_back(std::move(Output(this)));
}
Block::Block(const Block& block):
    parent{block.parent},
    name{block.name},
    inputs{block.inputs},
    outputs{block.outputs}
{
    for (Input &i : inputs)
    {
        i.parent = this;
        i.connection = nullptr;
    }
    for (Output &o : outputs)
    {
        o.parent = this;
        o.connections.clear();
    }
}
bool Block::is_connected()
{
    for (Input &i : inputs)
    {
        if (i.is_connected())
            return true;
    }
    for (Output &o : outputs)
    {
        if (o.is_connected())
            return true;
    }

    return false;
}
Input * Block::get_input(unsigned which)
{
    if (which <= inputs.size())
        return &*std::next(inputs.begin(), which);

    return nullptr;
}
Input * Block::get_input(std::string which)
{
    for (Input &i : inputs)
    {
        if (which == i.name)
            return &i;
    }

    return nullptr;
}
Output * Block::get_output(unsigned which)
{
    if (which <= outputs.size())
        return &*std::next(outputs.begin(), which);

    return nullptr;
}
Output * Block::get_output(std::string which)
{
    for (Output &o : outputs)
    {
        if (which == o.name)
            return &o;
    }

    return nullptr;
}
// void Block::rename_input(unsigned which, std::string name) {}
// void Block::rename_input(std::string which, std::string name) {}
// void Block::rename_output(unsigned which, std::string name) {}
// void Block::rename_output(std::string which, std::string name) {}
void Block::add_input(std::string name, std::string value)
{
    inputs.push_back(std::move(Input(this, name, value)));
}
void Block::add_output(std::string name)
{
    outputs.push_back(std::move(Output(this, name)));
}
// void Block::remove_input(unsigned which) {}
// void Block::remove_input(std::string name) {}


Atomic::Atomic(std::string n) : Block{n} {}
Atomic::Atomic(std::string n, int in, int out, std::string c) : Block{n, in, out}, code{c} {}

int Composite::instance_number = 0;
Composite::Composite() : Block{std::to_string(instance_number)} {++instance_number;}
Composite::Composite(std::string n) : Block{n} {++instance_number;}
Composite::Composite(int in, int out) : Block{std::to_string(instance_number), in, out} {++instance_number;}
Composite::Composite(std::string n, int in, int out) : Block{n, in, out} {++instance_number;}
void Composite::add_block(Block *block)
{
    // if (block->parent != this) // TODO ? - if it isn't already in this composite block - remove from parent first, then add here
    if (!block->parent)
    {
        blocks.push_back(block);
        block->parent = this;
    }
}
// void Composite::add_blocks(Block *block...) {}
void Composite::generate()
{
    if (!inputs.size() && !outputs.size())
    {
        // generate program
        std::cerr << "Generating program.\n";
        std::cerr << "Generation successful.\n";
    }
    else
        throw GenerationError("Application block must not have any ports.");
}


Library::Library()
{
    // TODO - read from file, no duplicate names!

    Atomic sub{"SUB", 2, 1, "output[0] = input[0] - input[1]"};
    atomic_blocks.emplace("SUB", std::move(sub));

    Atomic add{"ADD", 2, 1, "output[0] = input[0] + input[1]"};
    atomic_blocks.emplace("ADD", std::move(add));

    Composite app{"APP"};
    Atomic sub1{atomic_blocks.at("SUB")};
    sub1.get_input(0)->value = "100";
    sub1.get_input(1)->value = "40";
    app.add_block(&sub1);
    Atomic sub2{atomic_blocks.at("SUB")};
    sub2.get_input(0)->value = "10";
    sub2.get_input(1)->value = "1";
    app.add_block(&sub2);
    Atomic add1{atomic_blocks.at("ADD")};
    app.add_block(&add1);
    sub1.get_output()->connect(add1.get_input(0));
    sub2.get_output()->connect(add1.get_input(1));
    applications.emplace("APP", std::move(app));
}
Library::~Library()
{
    // TODO - save the current content into file
}
