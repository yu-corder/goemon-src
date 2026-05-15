# Goemon (五右衛門)

Goemon is a stack-machine-based virtual machine and compiler implemented in C. 
It is designed to explore low-level system architecture and compiler theory through a hands-on, minimalist approach.

## 🌟 Key Features

- **Stack-Machine Architecture**: A custom VM that executes instructions based on stack operations.
- **Recursive Descent Parsing**: A robust parsing engine where grammar rules are mapped to C functions, enabling advanced expression handling with operator precedence.
- **Full Arithmetic Support**: Supports complex expressions including addition, subtraction, multiplication, and division (e.g., `a = 1 + 3 * 8 / 2;`).
- **Variable Support**: Direct memory allocation and management for variables.
- **Centralized Bytecode Generation**: Uses an `emit_op` abstraction to ensure consistent and reliable bytecode output.

## 🛠️ Technical Stack

- **Language**: C
- **Architecture**: Stack Machine
- **Parsing Strategy**: Recursive Descent (Top-down parsing)

## 🚀 Getting Started

### ✨ Example

```text
b = 105 + 2 * 3;
print b;
```

```text
2207
```

### Compilation
You can compile the project using `gcc` directly or via the `Makefile`.

**Option 1: Using make (Recommended)**
```bash
make
```

## 📚 Technical Deep Dive (Blog)
- Blog: Yu-Syumilog
- https://yu-syumilog.com/
- Recommended Article:
  "How to Build a Programming Language #3"