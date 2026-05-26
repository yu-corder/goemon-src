# Goemon (五右衛門)

Goemon is a stack-machine-based virtual machine and compiler implemented in C.
It is designed to explore low-level system architecture and compiler theory through a hands-on, minimalist approach.

## 🌟 Key Features

- **Stack-Machine Architecture**: A custom VM that executes instructions based on stack operations.
- **Recursive Descent Parsing**: A robust parsing engine where grammar rules are mapped to C functions, enabling advanced expression handling with operator precedence.
- **Full Arithmetic Support**: Supports complex expressions including addition, subtraction, multiplication, and division.
- **Variable Support**: Direct memory allocation and management for variables.
- **Conditional Branches (if statements)**: Supports relational operators (e.g., <=, <) and conditional execution using OP_JZ (Jump if Zero).
- **Control Flow with Stack-Based Backpatching**: Implements a robust control flow engine that supports deeply nested if blocks by resolving jump addresses dynamically using a compilation stack.
- **Centralized Bytecode Generation**: Uses an emit_op abstraction to ensure consistent and reliable bytecode output.

## 🛠️ Technical Stack

- **Language**: C
- **Architecture**: Stack Machine
- **Parsing Strategy**: Recursive Descent (Top-down parsing)
- **Control Flow Strategy**: Two-pass Compilation with Stack-based Backpatching

## 🚀 Getting Started

### ✨ Example
```text
if (18 <= 18) {
    if (18 < 18) {
        print 10;
    }
    print 100;
}
print 1000;
```

```text
VM Output: 100
VM Output: 1000
```

### Compilation
You can compile the project using gcc directly or via the Makefile.

**Option 1: Using make (Recommended)**
```bash
make
```

## 📚 Technical Deep Dive (Blog)

- Blog: Yu-Syumilog
- https://yu-syumilog.com/
- Recommended Article:
"How to Build a Programming Language #3"