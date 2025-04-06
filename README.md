# Polynomial and Arithmetic Expression Compiler

This program provides two main functionalities:
  1. **Polynomial Evaluation** with variables `x` and `y`
  2. **Arithmetic Expression Parsing and Evaluation**

---

## Features

### 1. Polynomial Evaluation (Option 1)
- Accepts polynomial expressions with variables `x` and `y`.
- Supports basic operations: `+`, `-`, `*`, `/`.
- Prompts the user for values of `x` and `y`.
- Substitutes the values into the polynomial and evaluates the expression.
- Displays step-by-step computation instructions.

### 2. Arithmetic Expression Parser (Option 2)
- Parses arithmetic expressions with numbers and operators.
- Supports parentheses for grouping.
- Generates intermediate computation steps.
- Handles basic operations: `+`, `-`, `*`, `/`.

---

## How to Run

### 1. Compile the Program
Use a C++ compiler to compile the program. For example:                    
```bash
g++ -std=c++17 complier.cpp -o compiler
```

### 2. Run the Program
Execute the compiled program:
```bash
./compiler
```

### 3. Choose an Option
When prompted, select the mode:
- Enter `1` for **polynomial evaluation**.
- Enter `2` for **arithmetic expression parsing**.

---

## Examples

### Polynomial Evaluation Example
#### Input:
```
Enter '1' for polynomial evaluation or '2' for arithmetic expressions: 1
Enter polynomial in x and y (e.g., 3x^2 + 2y^2 - 5xy + 1): 3x^2 + 2y^2 - 12x - 12y + 30
Enter value of x: 2
Enter value of y: 3
```

#### Output:
```
Enter value for x: 2
Enter value for y: 3
Instructions and results:
MUL x x -> t0 = 4
MUL 3 t0 -> t1 = 12
MUL y y -> t2 = 9
MUL 2 t2 -> t3 = 18
MUL -12 x -> t4 = -24
MUL -12 y -> t5 = -36
ADD t1 t3 -> t6 = 30
ADD t6 t4 -> t7 = 6
ADD t7 t5 -> t8 = -30
ADD t8 30 -> t9 = 0
Final result: 0
```

---

### Arithmetic Expression Example
#### Input:
```
Enter '1' for polynomial evaluation or '2' for arithmetic expressions: 2
Enter an arithmetic expression: (3 + 5) * 2
```

#### Output:
```
Generated Instructions:
ADD 3 5 -> t0
MUL t0 2 -> t1
```

---

## Implementation Details

- Uses **recursive descent parsing** for arithmetic expressions.
- Generates intermediate computation steps for both arithmetic and polynomial expressions.
- Handles basic error checking for invalid input.
- Substitutes values for variables in polynomial evaluation.

---

## Future Improvements

- Add support for **exponents** in polynomials (e.g., `x^2`, `y^3`).
- Implement proper **polynomial parsing** and evaluation for more complex expressions.
- Add support for additional operators (e.g., modulo `%`, power `^`).
- Improve error handling and provide more descriptive error messages.

---

## Requirements

- **C++17** or later.
- A C++ compiler (e.g., `g++`).

---
