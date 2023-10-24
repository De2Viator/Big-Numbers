#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <variant>
#include <string>
#include <iomanip>
#include <bitset>

enum Byte_operations_with_two_numbers { AND_MODE, OR_MODE, XOR_MODE };
enum Shift_operation { SHIFT_LEFT, SHIFT_RIGHT };
enum Math_operation { ADD_OPERATION, SUB_OPERATION, MOD_OPERATION };

class Big_Number {
public:
    std::variant<std::vector<unsigned int>, std::string>  number;
    Big_Number() = default;

    [[nodiscard]] std::string get_hex() const {
        if(is_hex()) {
            return std::get<std::string>(number);
        } else {
            std::string result;
            std::vector<unsigned int> numb_arr = std::get<std::vector<unsigned int >>(number);
            for (int decimal_number : numb_arr) {
                result += std::to_string(decimal_number);
            }

            unsigned long long decimal_number = 0;
            for (char digit : result) {
                decimal_number = decimal_number * 10 + (digit - '0');
            }

            std::string hex_string;
            while (decimal_number > 0) {
                int remainder = decimal_number % 16;
                char hex_digit = (remainder < 10) ? ('0' + remainder) : ('A' + remainder - 10);
                hex_string = hex_digit + hex_string;
                decimal_number /= 16;
            }
            return hex_string;
        }
    }

    void set_array(std::vector<unsigned int> numbers) {
        number = std::move(numbers);
    }

    void set_hex(const std::string& hexed_number) {
        number = hexed_number;
    }

    [[nodiscard]] bool is_hex() const {
        return std::holds_alternative<std::string>(number);
    }

    static Big_Number INV(const Big_Number& number) {
        Big_Number inv_number;
        std::string hexed_number = number.get_hex();
        std::string inverted_hex = "";
        for (char c : hexed_number) {
            int num = std::stoi(std::string(1, c), nullptr, 16);
            int inverted_num = ~num & 0xF;
            inverted_hex += std::to_string(inverted_num);
        }
        inv_number.set_hex(inverted_hex);
        return inv_number;
    }

    static Big_Number XOR(const Big_Number& first_number, const Big_Number& second_number) {
        Big_Number xor_number;
        std::string result = make_byte_operation_with_two_number(first_number, second_number, XOR_MODE);
        xor_number.set_hex(result);
        return xor_number;
    }

    static Big_Number OR(const Big_Number& first_number, const Big_Number& second_number) {
        Big_Number or_number;
        std::string result = make_byte_operation_with_two_number(first_number, second_number, OR_MODE);
        or_number.set_hex(result);
        return or_number;
    }

    static Big_Number AND(const Big_Number& first_number, const Big_Number& second_number) {
        Big_Number and_number;
        std::string result = make_byte_operation_with_two_number(first_number, second_number, AND_MODE);
        and_number.set_hex(result);
        return and_number;
    }

    static Big_Number shift_r(const Big_Number& number, long long n) {
        Big_Number shifted_number;
        std::string result = make_shift(number, n, SHIFT_RIGHT);
        shifted_number.set_hex(result);
        return shifted_number;
    }

    static Big_Number shift_l(const Big_Number& number, long long n) {
        Big_Number shifted_number;
        std::string result = make_shift(number, n, SHIFT_LEFT);
        shifted_number.set_hex(result);
        return shifted_number;
    }

    static Big_Number ADD(const Big_Number& first_number, const Big_Number& second_number) {
        Big_Number add_number;
        std::string result = make_math_operation(first_number, second_number, ADD_OPERATION);
        add_number.set_hex(result);
        return add_number;
    }

    static Big_Number SUB(const Big_Number& first_number, const Big_Number& second_number) {
        Big_Number sub_number;
        std::string result = make_math_operation(first_number, second_number, SUB_OPERATION);
        sub_number.set_hex(result);
        return sub_number;
    }

    // HELPERS

    static std::string make_hex(unsigned long long number) {
        std::stringstream hex_stream;
        hex_stream << std::hex << number;
        return hex_stream.str();
    }

    static int hex_char_to_value(char c) {
        if (std::isxdigit(c)) {
            c = std::tolower(c);
            return std::isdigit(c) ? c - '0' : c - 'a' + 10;
        }
        return -1;
    }

    static char hex_value_to_char(int num) {
        return num < 10 ? static_cast<char>('0' + num) : static_cast<char>('a' + num - 10);
    }

private:
    [[nodiscard]] static std::string make_byte_operation_with_two_number(const Big_Number& first_number,
                                                                         const Big_Number& second_number,
                                                                         Byte_operations_with_two_numbers mode) {
        std::string first_hexed_number = first_number.get_hex();
        std::string second_hexed_number = second_number.get_hex();
        Big_Number result_number;

        if (first_hexed_number.length() != second_hexed_number.length()) {
            std::cerr << "Hexadecimal string lengths do not match." << std::endl;
            result_number.set_hex("0");
            return "";
        }

        std::string result;
        result.resize(first_hexed_number.length());

        for (long long i = 0; i < first_hexed_number.length(); i++) {
            int num1 = hex_char_to_value(first_hexed_number[i]);
            int num2 = hex_char_to_value(second_hexed_number[i]);
            int operationResult;

            if (mode == AND_MODE) {
                operationResult = num1 & num2;
            } else if (mode == XOR_MODE) {
                operationResult = num1 ^ num2;
            } else {
                operationResult = num1 | num2;
            }

            if (operationResult >= 0 && operationResult <= 15) {
                result[i] = hex_value_to_char(operationResult);
            }
        }

        return result;
    }

    [[nodiscard]] static std::string make_math_operation(const Big_Number& first_number,
                                                         const Big_Number& second_number,
                                                         Math_operation operation) {
        std::string result = "";
        int shift = 0;
        std::string hex_num1 = first_number.get_hex();
        std::string hex_num2 = second_number.get_hex();

        int len1 = hex_num1.length();
        int len2 = hex_num2.length();
        int max_length = std::max(len1, len2);
        bool is_sub = operation == SUB_OPERATION;

        std::string padded_num1 = std::string(max_length - len1, '0') + hex_num1;
        std::string padded_num2 = std::string(max_length - len2, '0') + hex_num2;

        for (int i = max_length - 1; i >= 0; i--) {
            int digit1 = std::isdigit(padded_num1[i]) ? padded_num1[i] - '0' : 10 + std::toupper(padded_num1[i]) - 'A';
            int digit2 = std::isdigit(padded_num2[i]) ? padded_num2[i] - '0' : 10 + std::toupper(padded_num2[i]) - 'A';
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
                result_operation = digit1 + digit2 + shift;
                shift = result_operation / 16;
                result_operation %= 16;
            }

            result = hex_value_to_char(result_operation) + result;
        }

        if (shift > 0 && !is_sub) {
            result = (char)(shift + '0') + result;
        } else {
            result.erase(0, std::min(result.find_first_not_of("0"), result.size() - 1));
        }
        return result;
    }

    [[nodiscard]] static std::string make_shift(const Big_Number& first_number,
                                                unsigned long long n,
                                                Shift_operation mode) {
        const int block_size = 8;
        std::string hex_number = first_number.get_hex();
        std::vector<std::string> hex_blocks;

        for (int i = hex_number.length(); i > 0; i -= block_size) {
            int start_pos = (i >= block_size) ? (i - block_size) : 0;
            hex_blocks.push_back(hex_number.substr(start_pos, block_size));
        }

        std::string result = "";
        for (const std::string& block : hex_blocks) {
            unsigned long long decimal_value;
            std::stringstream ss;
            ss << std::hex << block;
            ss >> decimal_value;

            if(mode == SHIFT_RIGHT) {
                decimal_value >>= n;
            } else {
                decimal_value <<= n;
            }
            std::stringstream hex_stream;
            hex_stream << std::hex << decimal_value;
            std::string hex_result = hex_stream.str();

            result = hex_result + result;
        }

        return result;
    }
};

int main() {
    //INV
    Big_Number number_for_inv;
    number_for_inv.set_hex("0");
    Big_Number inv_result = Big_Number::INV(number_for_inv);
    std::cout<<"INV: "<<inv_result.get_hex()<<std::endl; // Must be 15

    //XOR
    Big_Number number_for_xor;
    Big_Number second_number_for_xor;
    number_for_xor.set_hex("51bf608414ad5726a3c1bec098f77b1b54ffb2787f8d528a74c1d7fde6470ea4");
    second_number_for_xor.set_hex("403db8ad88a3932a0b7e8189aed9eeffb8121dfac05c3512fdb396dd73f6331c");
    Big_Number xor_result = Big_Number::XOR(number_for_xor,second_number_for_xor);
    std::cout<<"XOR: "<<xor_result.get_hex()<<std::endl;

    //OR
    Big_Number number_for_or;
    Big_Number second_number_for_or;
    number_for_or.set_hex("0110");
    second_number_for_or.set_hex("1010");
    Big_Number or_result = Big_Number::OR(number_for_or,second_number_for_or);
    std::cout<<"OR: "<<or_result.get_hex()<<std::endl; // Must be 1110

    //AND
    Big_Number number_for_and;
    Big_Number second_number_for_and;
    number_for_and.set_hex("1010");
    second_number_for_and.set_hex("1100");
    Big_Number and_result = Big_Number::AND(number_for_and,second_number_for_and);
    std::cout<<"AND: "<<and_result.get_hex()<<std::endl; // Must be 1000

    //shiftR
    Big_Number number_for_shift_r;
    number_for_shift_r.set_hex("11");
    Big_Number shift_r_result = Big_Number::shift_r(number_for_shift_r, 1);
    std::cout<<"SHIFT_R: "<<shift_r_result.get_hex()<<std::endl; // Must be 8

    //shiftL
    Big_Number number_for_shift_l;
    number_for_shift_l.set_hex("11");
    Big_Number shift_l_result = Big_Number::shift_l(number_for_shift_l, 1);
    std::cout<<"SHIFT_L: "<<shift_l_result.get_hex()<<std::endl; // Must be 22


    //ADD
    Big_Number number_for_adding;
    Big_Number second_number_for_adding;
    number_for_adding.set_hex("36f028580bb02cc8272a9a020f4200e346e276ae664e45ee80745574e2f5ab80");
    second_number_for_adding.set_hex("70983d692f648185febe6d6fa607630ae68649f7e6fc45b94680096c06e4fadb");
    Big_Number adding_result = Big_Number::ADD(number_for_adding, second_number_for_adding);
    std::cout<<"ADD: "<<adding_result.get_hex()<<std::endl;

    //SUB
    Big_Number number_for_sub;
    Big_Number second_number_for_sub;
    number_for_sub.set_hex("33ced2c76b26cae94e162c4c0d2c0ff7c13094b0185a3c122e732d5ba77efebc");
    second_number_for_sub.set_hex("22e962951cb6cd2ce279ab0e2095825c141d48ef3ca9dabf253e38760b57fe03");
    Big_Number sub_result = Big_Number::SUB(number_for_sub, second_number_for_sub);
    std::cout<<"SUB: "<<sub_result.get_hex()<<std::endl;
    return 0;
}