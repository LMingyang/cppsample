#include "vm.h"

#include <iostream>
#include <limits>

#include "util.h"


namespace vm {


vm_state create_vm(bool debug) {
    vm_state state;

    // enable vm debugging
    state.debug = debug;


    register_instruction(state, "PRINT", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"no return value when printing. pc="}
                               + std::to_string(vmstate.pc)};
        }
        std::cout << vmstate.stack.top() << std::endl;
        return true;
    });


    register_instruction(state, "LOAD_CONST", [](vm_state& vmstate, const item_t item) {
        vmstate.stack.push(item);
        return true;
    });

    register_instruction(state, "EXIT", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"no return value when exiting. pc="}
            + std::to_string(vmstate.pc)};
        }
        return false;
    });

    register_instruction(state, "POP", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"not enough stack when popping. pc="}
            + std::to_string(vmstate.pc)};
        }
        vmstate.stack.pop();
        return true;
    });

    register_instruction(state, "ADD", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.size() < 2) {
            throw vm_stackfail{std::string {"not enough stack when adding. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val1 = vmstate.stack.top();
        vmstate.stack.pop();
        item_t val2 = vmstate.stack.top();
        vmstate.stack.pop();
        vmstate.stack.push(val1 + val2);
        return true;
    });

    register_instruction(state, "DIV", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.size() < 2) {
            throw vm_stackfail{std::string {"not enough stack when dividing. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val1 = vmstate.stack.top();

        if(val1 == 0)
            throw div_by_zero{"div by zero"};

        vmstate.stack.pop();
        item_t val2 = vmstate.stack.top();
        vmstate.stack.pop();
        vmstate.stack.push(val2 / val1);
        return true;
    });

    register_instruction(state, "EQ", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.size() < 2) {
            throw vm_stackfail{std::string {"not enough stack when comparing. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val1 = vmstate.stack.top();
        vmstate.stack.pop();
        item_t val2 = vmstate.stack.top();
        vmstate.stack.pop();

        vmstate.stack.push(val1 == val2 ? 1 : 0);
        return true;
    });

    register_instruction(state, "NEQ", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.size() < 2) {
            throw vm_stackfail{std::string {"not enough stack when comparing. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val1 = vmstate.stack.top();
        vmstate.stack.pop();
        item_t val2 = vmstate.stack.top();
        vmstate.stack.pop();

        vmstate.stack.push(val1 == val2 ? 0 : 1);
        return true;
    });

    register_instruction(state, "DUP", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"not enough value when duplicating. pc="}
                               + std::to_string(vmstate.pc)};
        }

        vmstate.stack.push(vmstate.stack.top());
        return true;
    });

    register_instruction(state, "JMP", [](vm_state& vmstate, const item_t addr) {
        vmstate.pc = static_cast<size_t>(addr);
        return true;
    });

    register_instruction(state, "JMPZ", [](vm_state& vmstate, const item_t addr) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"not enough stack when consuming. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val = vmstate.stack.top();
        vmstate.stack.pop();
        if(val == 0)
            vmstate.pc = static_cast<size_t>(addr);
        return true;
    });

    register_instruction(state, "WRITE", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"no value when appending. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val = vmstate.stack.top();
        vmstate.out.append(std::to_string(val));

        return true;
    });

    register_instruction(state, "WRITE_CHAR", [](vm_state& vmstate, const item_t /*arg*/) {
        if(vmstate.stack.empty()) {
            throw vm_stackfail{std::string {"no value when appending. pc="}
                               + std::to_string(vmstate.pc)};
        }
        item_t val = vmstate.stack.top();
        char cha = static_cast<char>('0' + val - 48);
        vmstate.out += cha;
        return true;
    });

    return state;
}


void register_instruction(vm_state& state, std::string_view name,
                          const op_action_t& action) {

    size_t op_id = state.next_op_id;

    state.next_op_id++;

    state.instruction_ids.emplace(name,op_id);
    state.instruction_names.emplace(op_id,name);
    state.instruction_actions.emplace(op_id,action);

    assemble(state,name);
}


code_t assemble(const vm_state& state, std::string_view input_program) {
    code_t code;

    // convert each line separately
    for (auto& line : util::split(input_program, '\n')) {

        auto line_words = util::split(line, ' ');

        // only support instruction and one argument
        if (line_words.size() >= 3) {
            throw invalid_instruction{std::string{"more than one instruction argument: "} + line};
        }

        // look up instruction id
        auto& op_name = line_words[0];
        auto find_op_id = state.instruction_ids.find(op_name);
        if (find_op_id == std::end(state.instruction_ids)) {
            throw invalid_instruction{std::string{"unknown instruction: "} + op_name};
        }
        op_id_t op_id = find_op_id->second;

        // parse the argument
        item_t argument{0};
        if (line_words.size() == 2) {
            argument = std::stoll(line_words[1]);
        }

        // save the instruction to the code store
        code.emplace_back(op_id, argument);
    }

    return code;
}


std::tuple<item_t, std::string> run(vm_state& vm, const code_t& code) {

    if (vm.debug) {
        std::cout << "=== running vm ======================" << std::endl;
        std::cout << "disassembly of run code:" << std::endl;
        for (const auto &[op_id, arg] : code) {
            if (not vm.instruction_names.contains(op_id)) {
                std::cout << "could not disassemble - op_id unknown..." << std::endl;
                std::cout << "turning off debug mode." << std::endl;
                vm.debug = false;
                break;
            }
            std::cout << vm.instruction_names[op_id] << " " << arg << std::endl;
        }
        std::cout << "=== end of disassembly" << std::endl << std::endl;
    }

    // execution loop for the machine
    while (true) {

        if (vm.pc >= code.size()) {
            throw vm_segfault{std::string{"execution in valid place. pc="} + std::to_string(vm.pc)
            + " code size = " + std::to_string(code.size())};
        }
        auto& [op_id, arg] = code[vm.pc];

        if (vm.debug) {
            std::cout << "-- exec " << vm.instruction_names[op_id] << " arg=" << arg << " at pc=" << vm.pc << std::endl;
        }

        // increase the program counter so its value can be overwritten
        vm.pc += 1;

        // execute instruction and stop if the action returns false.
        auto action = vm.instruction_actions[op_id];
        if(!action(vm,arg))
            break;
    }

    return {vm.stack.top(), vm.out};
}


} // namespace vm
