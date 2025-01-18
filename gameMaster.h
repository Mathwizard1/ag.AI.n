#pragma once
#include "gameFunction.h"
#include <unordered_set>
//#include <functional>  // For std::function

// Template class to handle functions with arguments
/*template<typename... Args>
class FunctionHolder {
private:
    std::string functionName;                      // To store the name of the function
    std::function<void(Args...)> functionPtr;      // Function pointer with arguments (variadic template)

public:
    // Constructor
    FunctionHolder(const std::string& name, std::function<void(Args...)> func)
        : functionName(name), functionPtr(func) {}

    // Method to execute the function whose pointer is held, with arguments
    void execute(Args... args) const {
        if (functionPtr) {
            std::cout << "Executing function: " << functionName << std::endl;
            functionPtr(args...);  // Call the function with the provided arguments
        } else {
            std::cout << "No function assigned." << std::endl;
        }
    }

    // Method to change the function pointer and name
    void setFunction(const std::string& name, std::function<void(Args...)> func) {
        functionName = name;
        functionPtr = func;
    }
};

// Example functions with different signatures
void greet() {
    std::cout << "Hello, World!" << std::endl;
}

void farewell(const std::string& name) {
    std::cout << "Goodbye, " << name << "!" << std::endl;
}

void multiply(int a, int b) {
    std::cout << "Result: " << a * b << std::endl;
}*/

class gameMaster
{
private:
	void tokenPrinter(std::vector<std::string>& tokens);

	std::unordered_set<char> invalidChar = {'\n', '\r', '\t', '(', ')'};
	char delimiter = ' ';

	void tokenizer(std::string& instruction, std::vector<std::string>& tokensList);

public:
	gameMaster();
	void getCode(std::vector<std::string> InstructionList);

	void transfer();
};

