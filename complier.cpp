#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <algorithm>

// Structure to represent a custom instruction
struct Instruction
{
    std::string op;    // Operation name (e.g., "ADD", "MUL", etc.)
    std::string arg1;  // First operand
    std::string arg2;  // Second operand (optional)
    std::string dest;  // Destination temporary variable
};

class ExpressionParser
{
private:
    const char *exprInput;             // Pointer to input expression
    int index;                         // Current position in the input
    int tempCount;                     // Counter for generating temp variable names
    std::vector<Instruction> instrList; // List of generated instructions

    // Get current character
    char currentChar() const { return exprInput[index]; }

    // Advance to next character
    void advanceChar() { index++; }

    // Skip whitespace characters
    void skipSpaces()
    {
        while (currentChar() == ' ')
            advanceChar();
    }

    // Generate a new temporary variable name
    std::string newTempVar()
    {
        return "t" + std::to_string(tempCount++);
    }

    // Parse a numeric literal
    std::string parseNumber()
    {
        skipSpaces();
        std::string numStr;
        while (isdigit(currentChar()) || currentChar() == '.')
        {
            numStr += currentChar();
            advanceChar();
        }
        return numStr;
    }

    // Parse factor: number, variable, or parenthesized expression
    std::string parseFactor()
    {
        skipSpaces();
        std::string resultVar;

        if (currentChar() == '(')
        {
            advanceChar(); // Skip '('
            resultVar = parseExpression(); // Parse inside parentheses
            if (currentChar() == ')') advanceChar();
            else throw std::runtime_error("Missing closing parenthesis");
        }
        else if (isdigit(currentChar()) || currentChar() == '.')
        {
            // Numeric literal
            resultVar = parseNumber();
            skipSpaces();
            // Implicit MUL if followed by var or '('
            if (isalpha(currentChar()) || currentChar() == '(')
            {
                std::string rightVar = parseFactor();
                std::string tempVar = newTempVar();
                // MUL: multiply resultVar and rightVar -> tempVar
                instrList.push_back({"MUL", resultVar, rightVar, tempVar});
                resultVar = tempVar;
            }
        }
        else if (isalpha(currentChar()))
        {
            // Variable name
            while (isalpha(currentChar()))
            {
                resultVar += currentChar();
                advanceChar();
            }
            skipSpaces();
            // Implicit MUL if followed by var or '('
            if (isalpha(currentChar()) || currentChar() == '(')
            {
                std::string rightVar = parseFactor();
                std::string tempVar = newTempVar();
                // MUL: multiply resultVar and rightVar -> tempVar
                instrList.push_back({"MUL", resultVar, rightVar, tempVar});
                resultVar = tempVar;
            }
        }
        else
        {
            throw std::runtime_error(std::string("Invalid character: ") + currentChar());
        }

        return resultVar;
    }

    // Parse exponentiation '^'
    std::string parsePower()
    {
        std::string baseVar = parseFactor();
        skipSpaces();
        if (currentChar() == '^')
        {
            advanceChar(); // Skip '^'
            std::string expVar = parseFactor();
            std::string tempVar = newTempVar();
            // POW: raise baseVar to expVar -> tempVar
            instrList.push_back({"POW", baseVar, expVar, tempVar});
            return tempVar;
        }
        return baseVar;
    }

    // Parse multiplication and division
    std::string parseTerm()
    {
        std::string leftVar = parsePower();
        skipSpaces();
        while (currentChar() == '*' || currentChar() == '/')
        {
            char opChar = currentChar();
            advanceChar(); // Skip operator
            std::string rightVar = parsePower();
            std::string tempVar = newTempVar();
            if (opChar == '*')
            {
                // MUL: multiply leftVar and rightVar -> tempVar
                instrList.push_back({"MUL", leftVar, rightVar, tempVar});
            }
            else
            {
                // DIV: divide leftVar by rightVar -> tempVar
                instrList.push_back({"DIV", leftVar, rightVar, tempVar});
            }
            leftVar = tempVar;
            skipSpaces();
        }
        return leftVar;
    }

    // Parse addition and subtraction
    std::string parseExpression()
    {
        std::string leftVar = parseTerm();
        skipSpaces();
        while (currentChar() == '+' || currentChar() == '-')
        {
            char opChar = currentChar();
            advanceChar(); // Skip operator
            std::string rightVar = parseTerm();
            std::string tempVar = newTempVar();
            if (opChar == '+')
            {
                // ADD: add leftVar and rightVar -> tempVar
                instrList.push_back({"ADD", leftVar, rightVar, tempVar});
            }
            else
            {
                // SUB: subtract rightVar from leftVar -> tempVar
                instrList.push_back({"SUB", leftVar, rightVar, tempVar});
            }
            leftVar = tempVar;
            skipSpaces();
        }
        return leftVar;
    }

public:
    // Constructor: initialize parser with expression string
    ExpressionParser(const std::string &expr) : exprInput(expr.c_str()), index(0), tempCount(0) {}

    // Parse the expression and build instruction list
    void parse()
    {
        parseExpression();
        skipSpaces();
        if (currentChar() != '\0')
            throw std::runtime_error("Unexpected input after expression");
    }

    // Print generated instructions
    void printInstructions() const
    {
        std::cout << "Generated Instructions:\n";
        for (const auto &ins : instrList)
        {
            std::cout << ins.op << " " << ins.arg1;
            if (!ins.arg2.empty()) std::cout << " " << ins.arg2;
            std::cout << " -> " << ins.dest << "\n";
        }
    }

    // Access instruction list
    const std::vector<Instruction> &getInstructions() const { return instrList; }
};

// Get numeric value of operand (literal or variable)
double operandValue(const std::string &op, const std::unordered_map<std::string,double> &varMap)
{
    if (isdigit(op[0]) || (op[0]=='-' && isdigit(op[1])))
        return std::stod(op);
    return varMap.at(op);
}

// Evaluate a single instruction
double evalInstruction(const Instruction &ins, const std::unordered_map<std::string,double> &varMap)
{
    double v1 = operandValue(ins.arg1, varMap);
    double v2 = ins.arg2.empty() ? 0 : operandValue(ins.arg2, varMap);
    if (ins.op == "ADD") return v1 + v2;
    if (ins.op == "SUB") return v1 - v2;
    if (ins.op == "MUL") return v1 * v2;
    if (ins.op == "DIV") return v1 / v2;
    if (ins.op == "POW") return std::pow(v1, v2);
    throw std::runtime_error("Unknown operation: " + ins.op);
}

int main()
{
    try
    {
        int modeSelection;
        std::cout << "Select mode (1: arithmetic, 2: polynomial): ";
        std::cin >> modeSelection;
        std::cin.ignore(); // Ignore newline

        std::string exprStr;
        std::cout << (modeSelection == 1 ? "Enter arithmetic expression: " : "Enter polynomial expression: ");
        std::getline(std::cin, exprStr);

        ExpressionParser parser(exprStr);
        parser.parse();

        if (modeSelection == 2)
        {
            std::unordered_map<std::string,double> varValues;
            // Prompt user for each variable used
            for (const auto &ins : parser.getInstructions())
            {
                for (const auto &operand : {ins.arg1, ins.arg2})
                {
                    if (!operand.empty() && isalpha(operand[0]) && operand.size() == 1
                        && !varValues.count(operand))
                    {
                        std::cout << "Enter value for " << operand << ": ";
                        std::cin >> varValues[operand];
                    }
                }
            }
            // Evaluate and print results
            std::cout << "Instructions and results:\n";
            for (const auto &ins : parser.getInstructions())
            {
                double res = evalInstruction(ins, varValues);
                varValues[ins.dest] = res;
                std::cout << ins.op << " " << ins.arg1;
                if (!ins.arg2.empty()) std::cout << " " << ins.arg2;
                std::cout << " -> " << ins.dest << " = " << res << "\n";
            }
            // Final result
            std::string finalDest = parser.getInstructions().back().dest;
            std::cout << "Final result: " << varValues[finalDest] << "\n";
        }
        else
        {
            // Just print instructions for arithmetic mode
            parser.printInstructions();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }
    return 0;
}
