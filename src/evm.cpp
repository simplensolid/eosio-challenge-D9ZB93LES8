#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdint.h>

enum Error {
    MEMORY_EXAUSTED = 1,
    ILLEGAL_TARGET,
    ILLEGAL_UPDATE,
    INVALID_OPCODE,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    UNIMPLEMENTED,
};

class uint256_t {
private:
    static constexpr int W = 8;
    uint32_t data[W];
    inline int cmp(const uint256_t& v) const {
        for (int i = 0; i < W; i++) {
            if (data[i] < v.data[i]) return -1;
            if (data[i] > v.data[i]) return 1;
        }
        return 0;
    }
public:
    inline uint256_t() { for (int i = 0; i < W; i++) data[i] = 0; }
    inline uint256_t(uint32_t v) { data[0] = v; for (int i = 1; i < W; i++) data[i] = 0; }
    inline uint256_t(const uint256_t& v) { for (int i = 0; i < W; i++) data[i] = v.data[i]; }
    inline uint256_t& operator=(const uint256_t& v) { for (int i = 0; i < W; i++) data[i] = v.data[i]; return *this; }
    inline uint64_t cast32() const { return data[0]; }
    inline const uint256_t sigflip() const { uint256_t v = *this; v.data[W-1] ^= 0x80000000; }
    inline const uint256_t operator~() const { uint256_t v; for (int i = 0; i < W; i++) v.data[i] = ~data[i]; return v; }
    inline const uint256_t operator-() const { uint256_t v = ~(*this); return ++v; }
    inline uint256_t& operator++() { for (int i = 0; i < W; i++) if (++data[i] != 0) break; return *this; }
    inline uint256_t& operator--() { for (int i = 0; i < W; i++) if (data[i]-- != 0) break; return *this; }
    inline const uint256_t operator++(int) { const uint256_t v = *this; ++(*this); return v; }
    inline const uint256_t operator--(int) { const uint256_t v = *this; --(*this); return v; }
    inline uint256_t& operator+=(const uint256_t& v)
    {
        uint64_t carry = 0;
        for (int i = 0; i < W; i++)
        {
            uint64_t n = data[i] + v.data[i] + carry;
            data[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }
    inline uint256_t& operator-=(const uint256_t& v) { *this += -v; return *this; }
    inline uint256_t& operator*=(const uint256_t& v) { throw UNIMPLEMENTED; }
    inline uint256_t& operator/=(const uint256_t& v) { throw UNIMPLEMENTED; }
    inline uint256_t& operator%=(const uint256_t& v) { throw UNIMPLEMENTED; }
    inline uint256_t& operator&=(const uint256_t& v) { for (int i = 0; i < W; i++) data[i] &= v.data[i]; return *this; }
    inline uint256_t& operator|=(const uint256_t& v) { for (int i = 0; i < W; i++) data[i] |= v.data[i]; return *this; }
    inline uint256_t& operator^=(const uint256_t& v) { for (int i = 0; i < W; i++) data[i] ^= v.data[i]; return *this; }
    inline uint256_t& operator<<=(int n) {
        int index = n / 32;
        int shift = n % 32;
        for (int i = W - 1; i >= 0; i--) {
            uint32_t w = 0;
            if (i >= index) w |= data[i - index] << shift;
            if (i >= index + 1 && shift > 0) w |= data[i - index - 1] >> (32 - shift);
            data[i] = w;
        }
        return *this;
    }
    inline uint256_t& operator>>=(int n) {
        int index = n / 32;
        int shift = n % 32;
        for (int i = 0; i < W; i++) {
            uint32_t w = 0;
            if (W > i + index) w |= data[i + index] >> shift;
            if (W > i + index + 1 && shift > 0) w |= data[i + index + 1] << (32 - shift);
            data[i] = w;
        }
        return *this;
    }
    inline const uint256_t sigext(const uint256_t& v1) const { throw UNIMPLEMENTED; }
    friend inline const uint256_t operator+(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) += v2; }
    friend inline const uint256_t operator-(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) -= v2; }
    friend inline const uint256_t operator*(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) *= v2; }
    friend inline const uint256_t operator/(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) /= v2; }
    friend inline const uint256_t operator%(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) %= v2; }
    friend inline const uint256_t operator&(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) &= v2; }
    friend inline const uint256_t operator|(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) |= v2; }
    friend inline const uint256_t operator^(const uint256_t& v1, const uint256_t& v2) { return uint256_t(v1) ^= v2; }
    friend inline const uint256_t operator<<(const uint256_t& v, int n) { return uint256_t(v) <<= n; }
    friend inline const uint256_t operator>>(const uint256_t& v, int n) { return uint256_t(v) >>= n; }
    friend inline bool operator==(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) == 0; }
    friend inline bool operator!=(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) != 0; }
    friend inline bool operator<(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) < 0; }
    friend inline bool operator>(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) > 0; }
    friend inline bool operator<=(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) <= 0; }
    friend inline bool operator>=(const uint256_t& v1, const uint256_t& v2) { return v1.cmp(v2) >= 0; }
    friend std::ostream& operator<<(std::ostream&, const uint256_t&);
};

std::ostream &operator<<(std::ostream &os, const uint256_t &v) {
    for (int i = uint256_t::W - 1; i >= 0; i--) {
        os << std::hex << std::setw(8) << std::setfill('0') << v.data[i];
    }
    return os;
}

const uint256_t word(const uint8_t *data, int size)
{
    uint256_t v = 0;
    for (int i = 0; i < size; i++) { v <<= 8; v += data[i]; }
    return v;
}

void word(const uint256_t &v, uint8_t *data, int size)
{
    for (int i = 0; i < size; i++) {
        data[size - i - 1] = (v >> 8 * i).cast32() & 0xff;
    }
}

uint256_t sha3(uint8_t *buffer, uint32_t size)
{
    throw UNIMPLEMENTED;
}

enum Opcode : uint8_t {
    STOP = 0x00,
    ADD = 0x01,
    MUL = 0x02,
    SUB = 0x03,
    DIV = 0x04,
    SDIV = 0x05,
    MOD = 0x06,
    SMOD = 0x07,
    ADDMOD = 0x08,
    MULMOD = 0x09,
    EXP = 0x0a,
    SIGNEXTEND = 0x0b,
    // 0x0c .. 0x0f
    LT = 0x10,
    GT = 0x11,
    SLT = 0x12,
    SGT = 0x13,
    EQ = 0x14,
    ISZERO = 0x15,
    AND = 0x16,
    OR = 0x17,
    XOR = 0x18,
    NOT = 0x19,
    BYTE = 0x1a,
    SHL = 0x1b,
    SHR = 0x1c,
    SAR = 0x1d,
    // 0x1e .. 0x1f
    SHA3 = 0x20,
    // 0x21 .. 0x2f
    ADDRESS = 0x30,
    BALANCE = 0x31,
    ORIGIN = 0x32,
    CALLER = 0x33,
    CALLVALUE = 0x34,
    CALLDATALOAD = 0x35,
    CALLDATASIZE = 0x36,
    CALLDATACOPY = 0x37,
    CODESIZE = 0x38,
    CODECOPY = 0x39,
    GASPRICE = 0x3a,
    EXTCODESIZE = 0x3b,
    EXTCODECOPY = 0x3c,
    RETURNDATASIZE = 0x3d,
    RETURNDATACOPY = 0x3e,
    EXTCODEHASH = 0x3f,
    BLOCKHASH = 0x40,
    COINBASE = 0x41,
    TIMESTAMP = 0x42,
    NUMBER = 0x43,
    DIFFICULTY = 0x44,
    GASLIMIT = 0x45,
	CHAINID = 0x46,
	SELFBALANCE = 0x47,
    // 0x48 .. 0x4f
    POP = 0x50,
    MLOAD = 0x51,
    MSTORE = 0x52,
    MSTORE8 = 0x53,
    SLOAD = 0x54,
    SSTORE = 0x55,
    JUMP = 0x56,
    JUMPI = 0x57,
    PC = 0x58,
    MSIZE = 0x59,
    GAS = 0x5a,
    JUMPDEST = 0x5b,
    // 0x5c .. 0x5f
    PUSH1 = 0x60,
    PUSH2 = 0x61,
    PUSH3 = 0x62,
    PUSH4 = 0x63,
    PUSH5 = 0x64,
    PUSH6 = 0x65,
    PUSH7 = 0x66,
    PUSH8 = 0x67,
    PUSH9 = 0x68,
    PUSH10 = 0x69,
    PUSH11 = 0x6a,
    PUSH12 = 0x6b,
    PUSH13 = 0x6c,
    PUSH14 = 0x6d,
    PUSH15 = 0x6e,
    PUSH16 = 0x6f,
    PUSH17 = 0x70,
    PUSH18 = 0x71,
    PUSH19 = 0x72,
    PUSH20 = 0x73,
    PUSH21 = 0x74,
    PUSH22 = 0x75,
    PUSH23 = 0x76,
    PUSH24 = 0x77,
    PUSH25 = 0x78,
    PUSH26 = 0x79,
    PUSH27 = 0x7a,
    PUSH28 = 0x7b,
    PUSH29 = 0x7c,
    PUSH30 = 0x7d,
    PUSH31 = 0x7e,
    PUSH32 = 0x7f,
    DUP1 = 0x80,
    DUP2 = 0x81,
    DUP3 = 0x82,
    DUP4 = 0x83,
    DUP5 = 0x84,
    DUP6 = 0x85,
    DUP7 = 0x86,
    DUP8 = 0x87,
    DUP9 = 0x88,
    DUP10 = 0x89,
    DUP11 = 0x8a,
    DUP12 = 0x8b,
    DUP13 = 0x8c,
    DUP14 = 0x8d,
    DUP15 = 0x8e,
    DUP16 = 0x8f,
    SWAP1 = 0x90,
    SWAP2 = 0x91,
    SWAP3 = 0x92,
    SWAP4 = 0x93,
    SWAP5 = 0x94,
    SWAP6 = 0x95,
    SWAP7 = 0x96,
    SWAP8 = 0x97,
    SWAP9 = 0x98,
    SWAP10 = 0x99,
    SWAP11 = 0x9a,
    SWAP12 = 0x9b,
    SWAP13 = 0x9c,
    SWAP14 = 0x9d,
    SWAP15 = 0x9e,
    SWAP16 = 0x9f,
    LOG0 = 0xa0,
    LOG1 = 0xa1,
    LOG2 = 0xa2,
    LOG3 = 0xa3,
    LOG4 = 0xa4,
    // 0xa5 .. 0xef
    CREATE = 0xf0,
    CALL = 0xf1,
    CALLCODE = 0xf2,
    RETURN = 0xf3,
    DELEGATECALL = 0xf4,
    CREATE2 = 0xf5,
    // 0xf6 .. 0xf9
    STATICCALL = 0xfa,
    // 0xfb .. 0xfc
    REVERT = 0xfd,
    // 0xfe .. 0xfe
    SELFDESTRUCT = 0xff,
};

static const char *opcodes[256] = {
    "STOP", "ADD", "MUL", "SUB", "DIV", "SDIV", "MOD", "SMOD",
    "ADDMOD", "MULMOD", "EXP", "SIGNEXTEND", "?", "?", "?", "?",
    "LT", "GT", "SLT", "SGT", "EQ", "ISZERO", "AND", "OR",
    "XOR", "NOT", "BYTE", "SHL", "SHR", "SAR", "?", "?",
    "SHA3", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "ADDRESS", "BALANCE", "ORIGIN", "CALLER", "CALLVALUE", "CALLDATALOAD", "CALLDATASIZE", "CALLDATACOPY",
    "CODESIZE", "CODECOPY", "GASPRICE", "EXTCODESIZE", "EXTCODECOPY", "RETURNDATASIZE", "RETURNDATACOPY", "EXTCODEHASH",
    "BLOCKHASH", "COINBASE", "TIMESTAMP", "NUMBER", "DIFFICULTY", "GASLIMIT", "CHAINID", "SELFBALANCE",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "POP", "MLOAD", "MSTORE", "MSTORE8", "SLOAD", "SSTORE", "JUMP", "JUMPI",
    "PC", "MSIZE", "GAS", "JUMPDEST", "?", "?", "?", "?",
    "PUSH1", "PUSH2", "PUSH3", "PUSH4", "PUSH5", "PUSH6", "PUSH7", "PUSH8",
    "PUSH9", "PUSH10", "PUSH11", "PUSH12", "PUSH13", "PUSH14", "PUSH15", "PUSH16",
    "PUSH17", "PUSH18", "PUSH19", "PUSH20", "PUSH21", "PUSH22", "PUSH23", "PUSH24",
    "PUSH25", "PUSH26", "PUSH27", "PUSH28", "PUSH29", "PUSH30", "PUSH31", "PUSH32",
    "DUP1", "DUP2", "DUP3", "DUP4", "DUP5", "DUP6", "DUP7", "DUP8",
    "DUP9", "DUP10", "DUP11", "DUP12", "DUP13", "DUP14", "DUP15", "DUP16",
    "SWAP1", "SWAP2", "SWAP3", "SWAP4", "SWAP5", "SWAP6", "SWAP7", "SWAP8",
    "SWAP9", "SWAP10", "SWAP11", "SWAP12", "SWAP13", "SWAP14", "SWAP15", "SWAP16",
    "LOG0", "LOG1", "LOG2", "LOG3", "LOG4", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "CREATE", "CALL", "CALLCODE", "RETURN", "DELEGATECALL", "CREATE2", "?", "?",
    "?", "?", "STATICCALL", "?", "?", "REVERT", "?", "SELFDESTRUCT",
};

class Stack {
private:
    static constexpr int L = 1024;
    uint256_t data[L];
    int top = 0;
public:
    inline const uint256_t pop() { if (top == 0) throw STACK_UNDERFLOW;  return data[--top]; }
    inline void push(const uint256_t& v) { if (top == L) throw STACK_OVERFLOW; data[top++] = v; }
    inline uint256_t operator [](int i) const { return data[i < 0 ? top - i: i]; }
    inline uint256_t& operator [](int i) { return data[i < 0 ? top - i: i]; }
};

class Memory {
private:
    static constexpr int P = 4096;
    static constexpr int S = P / sizeof(uint8_t*);
    uint32_t limit = 0;
    uint32_t page_count = 0;
    uint8_t **pages;
public:
    ~Memory() {
        for (uint32_t i = 0; i < page_count; i++) delete pages[i];
        delete pages;
    }
    inline uint32_t size() const { return limit; }
    inline uint8_t operator [](uint32_t i) const {
        uint32_t page_index = i / P;
        uint32_t byte_index = i % P;
        if (page_index >= page_count) return 0;
        if (pages[page_index] == nullptr) return 0;
        return pages[page_index][byte_index];
    }
    inline uint8_t& operator [](uint32_t i) {
        uint32_t page_index = i / P;
        uint32_t byte_index = i % P;
        if (page_index >= page_count) {
            uint32_t new_page_count = ((page_index / S) + 1) * S;
            uint8_t **new_pages = new uint8_t*[new_page_count];
            if (new_pages == nullptr) throw MEMORY_EXAUSTED;
            for (uint32_t i = 0; i < page_count; i++) new_pages[i] = pages[i];
            for (uint32_t i = page_count; i < new_page_count; i++) new_pages[i] = nullptr;
            page_count = new_page_count;
            pages = new_pages;
        }
        if (pages[page_index] == nullptr) {
            pages[page_index] = new uint8_t[P]();
            if (pages[page_index] == nullptr) throw MEMORY_EXAUSTED;
        }
        if (i > limit) limit = i;
        return pages[page_index][byte_index];
    }
    inline uint256_t load(uint32_t offset) const {
        uint8_t buffer[32];
        dump(offset, 32, buffer);
        return word(buffer, 32);
    }
    inline void store(uint32_t offset, const uint256_t& v) {
        uint8_t buffer[32];
        word(v, buffer, 32);
        burn(offset, 32, buffer);
    }
    inline void dump(uint32_t offset, uint32_t size, uint8_t *buffer) const {
        for (uint32_t i = 0; i < size; i++) {
            buffer[i] = (*this)[offset+i];
        }
    }
    inline void burn(uint32_t offset, uint32_t size, const uint8_t *buffer) {
        for (uint32_t i = 0; i < size; i++) {
            (*this)[offset+i] = buffer[i];
        }
    }
};

uint256_t balance_of(uint256_t address)
{
    throw UNIMPLEMENTED;
}

uint32_t code_size_at(uint256_t address)
{
    throw UNIMPLEMENTED;
}

const uint8_t *code_at(uint256_t address)
{
    throw UNIMPLEMENTED;
}

uint32_t block_timestamp()
{
    throw UNIMPLEMENTED;
}

uint256_t block_coinbase()
{
    throw UNIMPLEMENTED;
}

uint256_t block_hash(uint32_t index)
{
    throw UNIMPLEMENTED;
}

uint256_t code_hash_at(uint256_t address)
{
    throw UNIMPLEMENTED;
}

uint32_t block_number()
{
    throw UNIMPLEMENTED;
}

uint256_t block_difficulty()
{
    throw UNIMPLEMENTED;
}

void vm_run()
{
    uint256_t gas;
    uint256_t gas_limit;
    uint256_t owner_address;
    uint256_t origin_address;
    uint256_t gas_price;
    uint8_t input[1];
    uint256_t caller_address;
    // transaction value
    const uint8_t code[] = { PUSH2, 0x00, 0x00, PUSH3, 0x00, 0x00, 0x00, ADD, PUSH1, 0x00, RETURN };
    uint32_t code_size = 11;
    // block header
    uint32_t call_depth;
    // permissions

    uint32_t pc = 0;
    Stack stack;
    Memory memory;
    for (;;) {
        uint8_t opc = pc <= code_size ? code[pc] : STOP;
        std::cout << opcodes[opc] << std::endl;
        switch (opc) {
        case STOP: { return; }
        case ADD: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 + v2); pc++; break; }
        case MUL: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 * v2); pc++; break; }
        case SUB: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 - v2); pc++; break; }
        case DIV: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 / v2); pc++; break; }
        case SDIV: throw UNIMPLEMENTED;
        case MOD: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 % v2); pc++; break; }
        case SMOD: throw UNIMPLEMENTED;
        case ADDMOD: { uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(); stack.push((v1 + v2) % v3); pc++; break; }
        case MULMOD: { uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(); stack.push((v1 * v2) % v3); pc++; break; }
        case EXP: throw UNIMPLEMENTED;
        case SIGNEXTEND: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v2.sigext(v1)); pc++; break; }
        case LT: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 < v2); pc++; break; }
        case GT: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 > v2); pc++; break; }
        case SLT: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1.sigflip() < v2.sigflip()); pc++; break; }
        case SGT: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1.sigflip() > v2.sigflip()); pc++; break; }
        case EQ: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 == v2); pc++; break; }
        case ISZERO: { uint256_t v1 = stack.pop(); stack.push(v1 == 0); pc++; break; }
        case AND: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 & v2); pc++; break; }
        case OR: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 | v2); pc++; break; }
        case XOR: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v1 ^ v2); pc++; break; }
        case NOT: { uint256_t v1 = stack.pop(), v2 = ~v1; stack.push(v2); pc++; break; }
        case BYTE: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint8_t buffer[32];
            word(v2, buffer, 32);
            stack.push(v1 < 32 ? buffer[v1.cast32()] : 0);
            pc++;
        }
        case SHL: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v2 << v1.cast32()); pc++; break; }
        case SHR: { uint256_t v1 = stack.pop(), v2 = stack.pop(); stack.push(v2 >> v1.cast32()); pc++; break; }
        case SAR: throw UNIMPLEMENTED;
        case SHA3: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            uint256_t v3 = sha3(buffer, size);
            stack.push(v3);
            pc++;
            break;
        }
        case ADDRESS: {
            uint256_t v1 = owner_address;
            stack.push(v1);
            pc++;
            break;
        }
        case BALANCE: {
            uint256_t v1 = stack.pop();
            uint256_t v2 = balance_of(v1);
            stack.push(v2);
            pc++;
            break;
        }
        case ORIGIN: {
            uint256_t v1 = origin_address;
            stack.push(v1);
            pc++;
            break;
        }
        case CALLER: {
            uint256_t v1 = caller_address;
            stack.push(v1);
            pc++;
            break;
        }
        case CALLVALUE: {
            uint256_t call_value = 0 /*getCallValue()*/;
            stack.push(call_value);
            pc++;
            break;
        }
        case CALLDATALOAD: {
            uint256_t v1 = stack.pop();
            uint256_t value = 0/*getDataValue(v1)*/;
            stack.push(value);
            pc++;
            break;
        }
        case CALLDATASIZE: {
            uint256_t data_size = 0/*getDataSize()*/;
            stack.push(data_size);
            pc++;
            break;
        }
        case CALLDATACOPY: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop();
            uint32_t offset1 = v1.cast32();
            uint32_t offset2 = v2.cast32();
            uint32_t size = v3.cast32();
            const uint8_t *data = nullptr/*getData()*/;
            uint32_t data_size = 0/*getDataSize()*/;
            if (offset2 + size > data_size) size = data_size - offset2;
            memory.burn(offset1, size, &data[offset2]);
            pc++;
            break;
        }
        case CODESIZE: {
            uint256_t v1 = code_size;
            stack.push(v1);
            pc++;
            break;
        }
        case CODECOPY: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop();
            uint32_t offset1 = v1.cast32();
            uint32_t offset2 = v2.cast32();
            uint32_t size = v3.cast32();
            if (offset2 + size > code_size) size = code_size - offset2;
            memory.burn(offset1, size, &code[offset2]);
            pc++;
            break;
        }
        case GASPRICE: {
            uint256_t v1 = gas_price;
            stack.push(v1);
            pc++;
            break;
        }
        case EXTCODESIZE: {
            uint256_t v1 = stack.pop();
            uint256_t v2 = code_size_at(v1);
            stack.push(v2);
            pc++;
            break;
        }
        case EXTCODECOPY: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(), v4 = stack.pop();
            uint32_t address = v1.cast32();
            uint32_t offset1 = v2.cast32();
            uint32_t offset2 = v3.cast32();
            uint32_t size = v4.cast32();
            const uint8_t *code = code_at(address);
            uint32_t code_size = code_size_at(address);
            if (offset2 + size > code_size) size = code_size - offset2;
            memory.burn(offset1, size, &code[offset2]);
            pc++;
            break;
        }
        case RETURNDATASIZE: {
            uint256_t return_size = 0/*getReturnDataBufferSize()*/;
            stack.push(return_size);
            pc++;
            break;
        }
        case RETURNDATACOPY: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop();
            uint32_t offset1 = v1.cast32();
            uint32_t offset2 = v2.cast32();
            uint32_t size = v3.cast32();
            uint32_t return_size = 0/*getReturnDataBufferSize()*/;
            const uint8_t *return_data = nullptr/*getReturnDataBuffer()*/;
            if (offset2 + size > return_size) size = return_size - offset2;
            memory.burn(offset1, size, &return_data[offset2]);
            pc++;
            break;
        }
        case EXTCODEHASH: {
            uint256_t v1 = stack.pop();
            uint256_t hash = code_hash_at(v1);
            stack.push(hash);
            pc++;
            break;
        }
        case BLOCKHASH: {
            uint256_t v1 = stack.pop();
            uint32_t index = v1.cast32();
            uint256_t hash = block_hash(index);
            stack.push(hash);
            pc++;
            break;
        }
        case COINBASE: {
            uint256_t coinbase = block_coinbase();
            stack.push(coinbase);
            pc++;
            break;
        }
        case TIMESTAMP: {
            uint32_t timestamp = block_timestamp();
            stack.push(timestamp);
            pc++;
            break;
        }
        case NUMBER: {
            uint32_t number = block_number();
            stack.push(number);
            pc++;
            break;
        }
        case DIFFICULTY: {
            uint256_t difficulty = block_difficulty();
            stack.push(difficulty);
            pc++;
            break;
        }
        case GASLIMIT: {
            uint256_t v1 = gas_limit;
            stack.push(v1);
            pc++;
            break;
        }
        case CHAINID: throw UNIMPLEMENTED;
        case SELFBALANCE: throw UNIMPLEMENTED;
        case POP: { stack.pop(); pc++; break; }
        case MLOAD: {
            uint256_t v1 = stack.pop();
            uint256_t v2 = memory.load(v1.cast32());
            stack.push(v2);
            pc++;
            break;
        }
        case MSTORE: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            memory.store(v1.cast32(), v2);
            pc++;
            break;
        }
        case MSTORE8: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint8_t buffer[1];
            buffer[0] = v2.cast32() & 0xff;
            memory.burn(v1.cast32(), 1, buffer);
            pc++;
            break;
        }
        case SLOAD: {
            uint256_t v1 = stack.pop();
            uint256_t value = 0/*storage.load(v1)*/;
            stack.push(value);
            pc++;
            break;
        }
        case SSTORE: {
            if (false/*isStaticCall()*/) throw ILLEGAL_UPDATE;
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            // storage.store(v1, v2);
            pc++;
            break;
        }
        case JUMP: {
            uint256_t v1 = stack.pop();
            pc = v1.cast32();
            uint8_t opc = pc <= code_size ? code[pc] : STOP;
            if (opc != JUMPDEST) throw ILLEGAL_TARGET;
            break;
        }
        case JUMPI: {
            uint256_t v1 = stack.pop();
            uint256_t v2 = stack.pop();
            pc = v1 != 0 ? v2.cast32() : pc + 1;
            break;
        }
        case PC: { stack.push(pc); pc++; break; }
        case MSIZE: { stack.push(memory.size()); pc++; break; }
        case GAS: { stack.push(gas); pc++; break; }
        case JUMPDEST: { pc++; break; }
        case PUSH1: { const int n = 1; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH2: { const int n = 2; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH3: { const int n = 3; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH4: { const int n = 4; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH5: { const int n = 5; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH6: { const int n = 6; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH7: { const int n = 7; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH8: { const int n = 8; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH9: { const int n = 9; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH10: { const int n = 10; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH11: { const int n = 11; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH12: { const int n = 12; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH13: { const int n = 13; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH14: { const int n = 14; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH15: { const int n = 15; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH16: { const int n = 16; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH17: { const int n = 17; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH18: { const int n = 18; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH19: { const int n = 19; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH20: { const int n = 20; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH21: { const int n = 21; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH22: { const int n = 22; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH23: { const int n = 23; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH24: { const int n = 24; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH25: { const int n = 25; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH26: { const int n = 26; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH27: { const int n = 27; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH28: { const int n = 28; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH29: { const int n = 29; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH30: { const int n = 30; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH31: { const int n = 31; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case PUSH32: { const int n = 32; uint256_t v1 = word(&code[pc+1], n); stack.push(v1); pc += 1 + n; break; }
        case DUP1: { uint256_t v1 = stack[-1]; stack.push(v1); pc++; break; }
        case DUP2: { uint256_t v1 = stack[-2]; stack.push(v1); pc++; break; }
        case DUP3: { uint256_t v1 = stack[-3]; stack.push(v1); pc++; break; }
        case DUP4: { uint256_t v1 = stack[-4]; stack.push(v1); pc++; break; }
        case DUP5: { uint256_t v1 = stack[-5]; stack.push(v1); pc++; break; }
        case DUP6: { uint256_t v1 = stack[-6]; stack.push(v1); pc++; break; }
        case DUP7: { uint256_t v1 = stack[-7]; stack.push(v1); pc++; break; }
        case DUP8: { uint256_t v1 = stack[-8]; stack.push(v1); pc++; break; }
        case DUP9: { uint256_t v1 = stack[-9]; stack.push(v1); pc++; break; }
        case DUP10: { uint256_t v1 = stack[-10]; stack.push(v1); pc++; break; }
        case DUP11: { uint256_t v1 = stack[-11]; stack.push(v1); pc++; break; }
        case DUP12: { uint256_t v1 = stack[-12]; stack.push(v1); pc++; break; }
        case DUP13: { uint256_t v1 = stack[-13]; stack.push(v1); pc++; break; }
        case DUP14: { uint256_t v1 = stack[-14]; stack.push(v1); pc++; break; }
        case DUP15: { uint256_t v1 = stack[-15]; stack.push(v1); pc++; break; }
        case DUP16: { uint256_t v1 = stack[-16]; stack.push(v1); pc++; break; }
        case SWAP1: { uint256_t v1 = stack[-1]; stack[-1] = stack[-2]; stack[-2] = v1; pc++; break; }
        case SWAP2: { uint256_t v1 = stack[-1]; stack[-1] = stack[-3]; stack[-3] = v1; pc++; break; }
        case SWAP3: { uint256_t v1 = stack[-1]; stack[-1] = stack[-4]; stack[-4] = v1; pc++; break; }
        case SWAP4: { uint256_t v1 = stack[-1]; stack[-1] = stack[-5]; stack[-5] = v1; pc++; break; }
        case SWAP5: { uint256_t v1 = stack[-1]; stack[-1] = stack[-6]; stack[-6] = v1; pc++; break; }
        case SWAP6: { uint256_t v1 = stack[-1]; stack[-1] = stack[-7]; stack[-7] = v1; pc++; break; }
        case SWAP7: { uint256_t v1 = stack[-1]; stack[-1] = stack[-8]; stack[-8] = v1; pc++; break; }
        case SWAP8: { uint256_t v1 = stack[-1]; stack[-1] = stack[-9]; stack[-9] = v1; pc++; break; }
        case SWAP9: { uint256_t v1 = stack[-1]; stack[-1] = stack[-10]; stack[-10] = v1; pc++; break; }
        case SWAP10: { uint256_t v1 = stack[-1]; stack[-1] = stack[-11]; stack[-11] = v1; pc++; break; }
        case SWAP11: { uint256_t v1 = stack[-1]; stack[-1] = stack[-12]; stack[-12] = v1; pc++; break; }
        case SWAP12: { uint256_t v1 = stack[-1]; stack[-1] = stack[-13]; stack[-13] = v1; pc++; break; }
        case SWAP13: { uint256_t v1 = stack[-1]; stack[-1] = stack[-14]; stack[-14] = v1; pc++; break; }
        case SWAP14: { uint256_t v1 = stack[-1]; stack[-1] = stack[-15]; stack[-15] = v1; pc++; break; }
        case SWAP15: { uint256_t v1 = stack[-1]; stack[-1] = stack[-16]; stack[-16] = v1; pc++; break; }
        case SWAP16: { uint256_t v1 = stack[-1]; stack[-1] = stack[-17]; stack[-17] = v1; pc++; break; }
        case LOG0: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            // log owner_address, buffer
            pc++;
            break;
        }
        case LOG1: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            // log owner_address, v3, buffer
            pc++;
            break;
        }
        case LOG2: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(), v4 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            // log owner_address, v3, v4, buffer
            pc++;
            break;
        }
        case LOG3: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(), v4 = stack.pop(), v5 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            // log owner_address, v3, v4, v5, buffer
            pc++;
            break;
        }
        case LOG4: {
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(), v4 = stack.pop(), v5 = stack.pop(), v6 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            // log owner_address, v3, v4, v5, v6, buffer
            pc++;
            break;
        }
        case CREATE: {
            if (false/*isStaticCall()*/) throw ILLEGAL_UPDATE;
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop();
            uint32_t offset = v2.cast32();
            uint32_t size = v3.cast32();
            // create_contract(v1, offset, size);
            pc++;
            break;
        }
        case CALL: throw UNIMPLEMENTED;
        case CALLCODE: throw UNIMPLEMENTED;
        case RETURN: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            pc++;
            return;
        }
        case DELEGATECALL: throw UNIMPLEMENTED;
        case CREATE2: {
            if (false/*isStaticCall()*/) throw ILLEGAL_UPDATE;
            uint256_t v1 = stack.pop(), v2 = stack.pop(), v3 = stack.pop(), v4 = stack.pop();
            uint32_t offset = v2.cast32();
            uint32_t size = v3.cast32();
            // create_contract(v1, offset, size, v4);
            pc++;
            break;
        }
        case STATICCALL: throw UNIMPLEMENTED;
        case REVERT: {
            uint256_t v1 = stack.pop(), v2 = stack.pop();
            uint32_t offset = v1.cast32(), size = v2.cast32();
            uint8_t *buffer = new uint8_t[size];
            if (buffer == nullptr) throw MEMORY_EXAUSTED;
            memory.dump(offset, size, buffer);
            pc++;
            return;
        }
        case SELFDESTRUCT: { return; }
        default: throw INVALID_OPCODE;
        }
    }
}

int main(int argc, char *argv[])
{
    uint256_t x = 0xa8b4c2d1;
    std::cout << x << std::endl;
    std::cout << (x << (217)) << std::endl;
    std::cout << ((x << (217)) >> 219) << std::endl;
    try {
        vm_run();
    } catch (Error e) {
        std::cout << "error: " << e << std::endl;
    }
    return 0;
}
