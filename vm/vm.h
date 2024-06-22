#pragma once

#include <cstdint>
#include <functional>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace vm {


using item_t = int64_t;


using op_id_t = size_t;


// instruction with argument
using op_t = std::pair<op_id_t, item_t>;


// forward declaration
struct vm_state;

/**
 *
 * @param args: vmstate and the operation argument
 * @return true if the VM keep running on after a instruction
 */
using op_action_t = std::function<bool(vm_state&, const item_t)>;

/**
 * stores all assembled instructions
 */
using code_t = std::vector<op_t>;


struct vm_state {
    /**
     * current program code
     */
    size_t pc = 0;

    /**
     * the next instruction id
     */
    size_t next_op_id = 0;

    /**
     * main execution state stack
     */
    std::stack<item_t> stack;

    /**
     * mapping of operation id to instruction name and action
     */
    std::unordered_map<std::string, op_id_t> instruction_ids;

    std::unordered_map<op_id_t, std::string> instruction_names;

    std::unordered_map<op_id_t, op_action_t> instruction_actions;

    /**
     * vm debugging
     */
    bool debug = false;


    std::string out;
};


///////////////////////////////////////////////////////////////////////////////
// definition of the VM API

/**
 * create a vm with all available instructions registered
 *
 * @return a new vm state with instructions
 */
vm_state create_vm(bool debug = false);

/**
 * convert the instruction string to executable vm code
 *
 * @param vm: which vm to use
 * @param input_program: executable instructions
 * @return executable code
 */
code_t assemble(const vm_state& vm, std::string_view input_program);

/**
 * register a new instruction to a vm
 * @param vm: vm to register
 * @param name: the textual identifier of instruction
 * @param action: the function to run
 */
void register_instruction(vm_state& vm, std::string_view name,
                          const op_action_t &action);


/**
 * execute the vm
 * @return {last TOS item, result string from WRITE instructions}
 */
std::tuple<item_t, std::string> run(vm_state& vm, const code_t &code);


/**
 * exception types
 */
struct div_by_zero : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// invalid memory address
struct vm_segfault : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// stack content is not as expected
struct vm_stackfail : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// instruction could not be decoded
struct invalid_instruction : std::runtime_error {
    using std::runtime_error::runtime_error;
};


} // namespace vm
