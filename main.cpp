#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <variant>
#include <string>
#include <iomanip>
#include <bitset>

enum ByteOperationsWithTwoNumbers{AND_MODE, OR_MODE, XOR_MODE};
enum ShiftOperation {SHIFT_LEFT, SHIFT_RIGHT};
enum MathOperation {ADD_OPERATION, SUB_OPERATION, MOD_OPERATION};
class BigNumber {
public:
    std::variant<std::vector <unsigned int>, std::string>  number;
    BigNumber() = default;

    [[nodiscard]] std::string getHex() const {
        if(isHex()) {
            return std::get<std::string>(number);
        } else {
            std::string result;
            std::vector<unsigned int> numb_arr = std::get<std::vector<unsigned int >>(number);
            for (int decimalNumber : numb_arr) {
                result += std::to_string(decimalNumber);
            }

            unsigned long long decimalNumber = 0;
            for (char digit : result) {
                decimalNumber = decimalNumber * 10 + (digit - '0');
            }

            std::string hexString;
            while (decimalNumber > 0) {
                int remainder = decimalNumber % 16;
                char hexDigit = (remainder < 10) ? ('0' + remainder) : ('A' + remainder - 10);
                hexString = hexDigit + hexString;
                decimalNumber /= 16;
            }
            return hexString;
        }
    }

    void setArray(std::vector<unsigned int> numbers) {
        number = std::move(numbers);
    }

    void setHex(const std::string& hexedNumber) {
        number = hexedNumber;
    }

    [[nodiscard]] bool isHex() const {
        return std::holds_alternative<std::string>(number);
    }

    static BigNumber INV(const BigNumber& number) {
        BigNumber inv_number;
        std::string hexed_number = number.getHex();
        std::string inverted_hex = "";
        for (char c:hexed_number) {
            int num = std::stoi(std::string(1, c), nullptr, 16);
            int invertedNum = ~num & 0xF;
            inverted_hex += std::to_string(invertedNum);
        }
        inv_number.setHex(inverted_hex);
        return inv_number;
    }

    static BigNumber XOR(const BigNumber& first_number, const BigNumber& second_number) {
        BigNumber xor_number;
        std::string result = makeByteOperationWithTwoNumber(first_number,second_number, XOR_MODE);
        xor_number.setHex(result);
        return xor_number;
    }

    static BigNumber OR(const BigNumber& first_number, const BigNumber& second_number) {
        BigNumber or_number;
        std::string result = makeByteOperationWithTwoNumber(first_number,second_number, OR_MODE);
        or_number.setHex(result);
        return or_number;
    }

    static BigNumber AND(const BigNumber& first_number, const BigNumber& second_number) {
        BigNumber and_number;
        std::string result = makeByteOperationWithTwoNumber(first_number,second_number, AND_MODE);
        and_number.setHex(result);
        return and_number;
    }

    static BigNumber shiftR(const BigNumber& number, long long n) {
        BigNumber shifted_number;
        std::string result = makeShift(number, n, SHIFT_RIGHT);
        shifted_number.setHex(result);
        return shifted_number;
    }

    static BigNumber shiftL(const BigNumber& number, long long n) {
        BigNumber shifted_number;
        std::string result = makeShift(number, n, SHIFT_LEFT);
        shifted_number.setHex(result);
        return shifted_number;
    }

    static BigNumber ADD(const BigNumber& first_number, const BigNumber& second_number) {
        BigNumber add_number;
        std::string result = makeMathOperation(first_number,second_number, ADD_OPERATION);
        add_number.setHex(result);
        return add_number;
    }

    static BigNumber SUB(const BigNumber& first_number, const BigNumber& second_number) {
            BigNumber sub_number;
            std::string result = makeMathOperation(first_number, second_number, SUB_OPERATION);
            sub_number.setHex(result);
            return sub_number;
    }


    //HELPERS

    static std::string makeHex(unsigned long long number) {
        std::stringstream hex_stream;
        hex_stream << std::hex << number;
        return hex_stream.str();
    }

    static int HexCharToValue(char c) {
        if (std::isxdigit(c)) {
            c = std::tolower(c);
            return std::isdigit(c) ? c - '0' : c - 'a' + 10;
        }
        return -1;
    }

    static char HexValueToChar(int num) {
        return num < 10 ? static_cast<char>('0' + num) : static_cast<char>('a' + num - 10);
    }
private:

    [[nodiscard]] static std::string makeByteOperationWithTwoNumber(const BigNumber& first_number,
                                                             const BigNumber& second_number,
                                                             ByteOperationsWithTwoNumbers mode) {
        std::string first_hexed_number = first_number.getHex();
        std::string second_hexed_number = second_number.getHex();
        BigNumber result_number;

        if (first_hexed_number.length() != second_hexed_number.length()) {
            std::cerr << "Hexadecimal string lengths do not match." << std::endl;
            result_number.setHex("0");
            return "";
        }

        std::string result;
        result.resize(first_hexed_number.length());

        for (long long i = 0; i < first_hexed_number.length(); i++) {
            int num1 = HexCharToValue(first_hexed_number[i]);
            int num2 = HexCharToValue(second_hexed_number[i]);
            int operationResult;

            if(mode == AND_MODE) {
                operationResult = num1 & num2;
            } else if(mode == XOR_MODE) {
                operationResult = num1 ^ num2;
            } else {
                operationResult = num1 | num2;
            }

            if (operationResult >= 0 && operationResult <= 15) {
                result[i] = HexValueToChar(operationResult);
            }
        }

        return result;
    }

    [[nodiscard]] static std::string makeMathOperation(const BigNumber& first_number,
                                                       const BigNumber& second_number,
                                                       MathOperation operation) {
        std::string result = "";
        int shift = 0;
        std::string hexNum1 = first_number.getHex();
        std::string hexNum2 = second_number.getHex();

        int len1 = hexNum1.length();
        int len2 = hexNum2.length();
        int maxLength = std::max(len1, len2);
        bool isSub = operation == SUB_OPERATION;

        std::string paddedNum1 = std::string(maxLength - len1, '0') + hexNum1;
        std::string paddedNum2 = std::string(maxLength - len2, '0') + hexNum2;

        for (int i = maxLength - 1; i >= 0; i--) {
            int digit1 = std::isdigit(paddedNum1[i]) ? paddedNum1[i] - '0' : 10 + std::toupper(paddedNum1[i]) - 'A';
            int digit2 = std::isdigit(paddedNum2[i]) ? paddedNum2[i] - '0' : 10 + std::toupper(paddedNum2[i]) - 'A';
            int result_operation;

            if(operation == SUB_OPERATION) {
                result_operation = digit1 - digit2 - shift;
                if (result_operation < 0) {
                    shift = 1;
                    result_operation += 16;
                } else {
                    shift = 0;
                }
            } else if (operation == ADD_OPERATION) {
                result_operation= digit1 + digit2 + shift;
                shift = result_operation / 16;
                result_operation %= 16;
            }

            result = HexValueToChar(result_operation) + result;
        }
        if (shift > 0 && !isSub) {
            result = (char)(shift + '0') + result;
        } else {
            result.erase(0, std::min(result.find_first_not_of("0"), result.size() - 1));
        }
        return result;
    }

    [[nodiscard]] static std::string makeShift(const BigNumber& first_number,
                                               unsigned long long n,
                                               ShiftOperation mode) {
        const int blockSize = 8;
        std::string hexNumber = first_number.getHex();
        std::vector<std::string> hexBlocks;

        for (int i = hexNumber.length(); i > 0; i -= blockSize) {
            int startPos = (i >= blockSize) ? (i - blockSize) : 0;
            hexBlocks.push_back(hexNumber.substr(startPos, blockSize));
        }

        std::string result = "";
        for (const std::string& block : hexBlocks) {
            unsigned long long decimalValue;
            std::stringstream ss;
            ss << std::hex << block;
            ss >> decimalValue;

            if(mode == SHIFT_RIGHT) {
                decimalValue >>= n;
            } else {
                decimalValue <<= n;
            }
            std::stringstream hexStream;
            hexStream << std::hex << decimalValue;
            std::string hexResult = hexStream.str();

            result = hexResult + result;
        }

        return result;
    }
};

int main() {
    return 0;
}
