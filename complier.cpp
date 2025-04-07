#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <algorithm>
using namespace std;

// Structure to represent a custom instruction
typedef struct Instruction {
    string op;    // Operation name (e.g., "ADD", "MUL", etc.)
    string arg1;  // First operand
    string arg2;  // Second operand (optional)
    string dest;  // Destination temporary variable
} Instruction;

class ExpressionParser {
private:
    const char *exprInput;             // Input expression
    int index;                         // Current parsing position
    int tempCount;                     // Counter for temp variables
    vector<Instruction> instrList;     // Generated instructions

    // Utilities
    char currentChar() const { return exprInput[index]; }
    void advanceChar() { index++; }
    void skipSpaces() {
        while (currentChar() == ' ') advanceChar();
    }
    string newTempVar() { return "t" + to_string(tempCount++); }

    // Parse numeric literal
    string parseNumber() {
        skipSpaces();
        string num;
        while (isdigit(currentChar()) || currentChar() == '.') {
            num += currentChar();
            advanceChar();
        }
        return num;
    }

    // Parse factor: number, variable, parenthesis, or raghav(a,b)
    string parseFactor() {
        skipSpaces();
        string result;

        if (currentChar() == '(') {
            advanceChar();
            result = parseExpression();
            if (currentChar() == ')') advanceChar();
            else throw runtime_error("Missing closing parenthesis");
        }
        else if (isalpha(currentChar())) {
            // Parse identifier
            string ident;
            while (isalpha(currentChar())) {
                ident += currentChar();
                advanceChar();
            }
            skipSpaces();
            // Handle raghav(a,b)
            if (ident == "raghav" && currentChar() == '(') {
                advanceChar(); // skip '('
                string arg1 = parseExpression();
                skipSpaces();
                if (currentChar() == ',') advanceChar();
                else throw runtime_error("Expected ',' in raghav");
                string arg2 = parseExpression();
                skipSpaces();
                if (currentChar() == ')') advanceChar();
                else throw runtime_error("Missing closing parenthesis in raghav");
                // Emit RAGHAV instruction: compute (a + b)^2
                string temp = newTempVar();
                instrList.push_back({"RAGHAV", arg1, arg2, temp});
                result = temp;
            }
            else {
                // Regular variable
                result = ident;
                // Implicit multiplication
                if (isalpha(currentChar()) || currentChar() == '(') {
                    string right = parseFactor();
                    string temp = newTempVar();
                    instrList.push_back({"MUL", result, right, temp});
                    result = temp;
                }
            }
        }
        else if (isdigit(currentChar()) || currentChar() == '.') {
            result = parseNumber();
            skipSpaces();
            // Implicit multiplication
            if (isalpha(currentChar()) || currentChar() == '(') {
                string right = parseFactor();
                string temp = newTempVar();
                instrList.push_back({"MUL", result, right, temp});
                result = temp;
            }
        }
        else {
            throw runtime_error(string("Invalid character: ") + currentChar());
        }
        return result;
    }

    // Parse exponentiation
    string parsePower() {
        string base = parseFactor();
        skipSpaces();
        if (currentChar() == '^') {
            advanceChar();
            string exp = parseFactor();
            string temp = newTempVar();
            instrList.push_back({"POW", base, exp, temp});
            return temp;
        }
        return base;
    }

    // Parse * and /
    string parseTerm() {
        string left = parsePower();
        skipSpaces();
        while (currentChar() == '*' || currentChar() == '/') {
            char op = currentChar(); advanceChar();
            string right = parsePower();
            string temp = newTempVar();
            instrList.push_back({op=='*'?"MUL":"DIV", left, right, temp});
            left = temp;
            skipSpaces();
        }
        return left;
    }

    // Parse + and -
    string parseExpression() {
        string left = parseTerm();
        skipSpaces();
        while (currentChar() == '+' || currentChar() == '-') {
            char op = currentChar(); advanceChar();
            string right = parseTerm();
            string temp = newTempVar();
            instrList.push_back({op=='+'?"ADD":"SUB", left, right, temp});
            left = temp;
            skipSpaces();
        }
        return left;
    }

public:
    ExpressionParser(const string &expr)
        : exprInput(expr.c_str()), index(0), tempCount(0) {}

    void parse() {
        parseExpression();
        skipSpaces();
        if (currentChar() != '\0') throw runtime_error("Unexpected input after expression");
    }

    void printInstructions() const {
        cout << "Generated Instructions:\n";
        for (const auto &ins : instrList) {
            cout << ins.op << " " << ins.arg1;
            if (!ins.arg2.empty()) cout << " " << ins.arg2;
            cout << " -> " << ins.dest << "\n";
        }
    }

    const vector<Instruction>& getInstructions() const { return instrList; }
};

// Convert operand to numeric value
static double operandValue(const string &op, const unordered_map<string,double> &vars) {
    if (isdigit(op[0]) || (op[0]=='-' && isdigit(op[1]))) return stod(op);
    return vars.at(op);
}

// Evaluate one instruction
static double evalInstruction(const Instruction &ins, const unordered_map<string,double> &vars) {
    double v1 = operandValue(ins.arg1, vars);
    double v2 = ins.arg2.empty()?0:operandValue(ins.arg2, vars);
    if (ins.op == "ADD")  return v1 + v2;
    if (ins.op == "SUB")  return v1 - v2;
    if (ins.op == "MUL")  return v1 * v2;
    if (ins.op == "DIV")  return v1 / v2;
    if (ins.op == "POW")  return pow(v1, v2);
    if (ins.op == "RAGHAV") return pow(v1 + v2, 2); // Compute (a + b)^2
    throw runtime_error("Unknown operation: " + ins.op);
}

int main() {
    try {
        int mode;
        cout << "Select mode (1: arithmetic, 2: polynomial): ";
        cin >> mode; cin.ignore();

        cout << (mode==1?"Enter arithmetic expression: ":"Enter polynomial expression: ");
        string expr; getline(cin, expr);

        ExpressionParser parser(expr);
        parser.parse();

        if (mode == 2) {
            unordered_map<string,double> vars;
            // Prompt for variables
            for (const auto &ins : parser.getInstructions()) {
                for (const auto &op : {ins.arg1, ins.arg2}) {
                    if (!op.empty() && isalpha(op[0]) && op.size()==1 && !vars.count(op)) {
                        cout << "Enter value for " << op << ": ";
                        cin >> vars[op];
                    }
                }
            }
            // Evaluate and display
            cout << "Instructions and results:\n";
            for (const auto &ins : parser.getInstructions()) {
                double res = evalInstruction(ins, vars);
                vars[ins.dest] = res;
                cout << ins.op << " " << ins.arg1;
                if (!ins.arg2.empty()) cout << " " << ins.arg2;
                cout << " -> " << ins.dest << " = " << res << "\n";
            }
            // Final result
            string last = parser.getInstructions().back().dest;
            cout << "Final result: " << vars[last] << "\n";
        } else {
            parser.printInstructions();
        }
    } catch (const exception &e) {
        cerr << e.what() << "\n";
    }
    return 0;
}
