// NotC Virtual Machine
// Copyright (c) 2023, Fuechs. All rights reserved.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


#define DEBUG_TRACE_EXECUTION

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type *) reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, newSize);
    if (result == NULL)
        exit(1);
    return result;
}

typedef enum ValueType_ENUM {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
} ValueType;

// typedef double Value;

typedef struct Value_STRUCT {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NULL(value)      ((value).type == VAL_NULL)
#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)

#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)

#define BOOL_VAL(value)     ((Value) {VAL_BOOL, {.boolean = value}})
#define NULL_VAL            ((Value) {VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value)   ((Value) {VAL_NUMBER, {.number = value}})

typedef struct ValueArray_STRUCT{
    int capacity;
    int count;
    Value *values;
} ValueArray;

void initValueArray(ValueArray *array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:      printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NULL:      printf("null"); break;
        case VAL_NUMBER:    printf("%g", AS_NUMBER(value)); break;
    }
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type)
        return false;

    switch (a.type) {
        case VAL_BOOL:      return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NULL:      return true;
        case VAL_NUMBER:    return AS_NUMBER(a) == AS_NUMBER(b);
        default:            return false;
    }
}

typedef enum OpCode_ENUM {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct Chunk_STRUCT {
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int addConstant(Chunk *chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void writeConstant(Chunk *chunk, Value value, int line) {
    int index = addConstant(chunk, value);
    if (index < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t) index, line);
    } else {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunk(chunk, (uint8_t) (index & 0xff), line);
        writeChunk(chunk, (uint8_t) ((index >> 8) & 0xff), line);
        writeChunk(chunk, (uint8_t) ((index >> 16) & 0xff), line);
    }
}

static int simpleInstruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
} 

static int constantInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int longConstantInstruction(const char *name, Chunk *chunk, int offset) {
    uint32_t constant = chunk->code[offset + 1] 
                        | (chunk->code[offset + 2] << 8) 
                        | (chunk->code[offset + 3] << 16);
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 4;
}

int disassembleInstruction(Chunk *chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) 
        printf("   | ");
    else
        printf("%4d ", chunk->lines[offset]);
    
    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:       return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:  return longConstantInstruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_NULL:           return simpleInstruction("OP_NULL", offset);
        case OP_TRUE:           return simpleInstruction("OP_TRUE", offset);
        case OP_FALSE:          return simpleInstruction("OP_FALSE", offset);
        case OP_EQUAL:          return simpleInstruction("OP_EQUAL", offset);
        case OP_GREATER:        return simpleInstruction("OP_GREATER", offset);
        case OP_LESS:           return simpleInstruction("OP_LESS", offset);
        case OP_ADD:            return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:       return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:       return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:         return simpleInstruction("OP_DIVIDE", offset);
        case OP_NOT:            return simpleInstruction("OP_NOT", offset);
        case OP_NEGATE:         return simpleInstruction("OP_NEGATE", offset);
        case OP_RETURN:         return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

void disassembleChunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;)
        offset = disassembleInstruction(chunk, offset);
}

#define STACK_MAX 256

typedef struct VM_STRUCT {
    Chunk *chunk;
    uint8_t *ip;
    Value stack[STACK_MAX];
    Value *stackTop;
} VM;

typedef enum InterpretResult_ENUM {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in source\n", line);
    resetStack();
}

void initVM() {
    resetStack();
}

void freeVM() {

}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

static Value peek(int distance) {
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
    return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static InterpretResult run() {
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(valueType, op) \
        do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(pop()); \
        push(valueType(a op b)); \
        } while (false)

    for (;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int) (vm.ip - vm.chunk->code));
      
        #endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_NULL:       push(NULL_VAL); break;
            case OP_TRUE:       push(BOOL_VAL(true)); break;
            case OP_FALSE:      push(BOOL_VAL(false)); break;
            case OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(BOOL_VAL(valuesEqual (a, b)));
                break;
            }
            case OP_GREATER:    BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:       BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD:        BINARY_OP(NUMBER_VAL, +); break;
            case OP_SUBTRACT:   BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY:   BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:     BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT:        push(BOOL_VAL(isFalsey(pop()))); break;
            case OP_NEGATE:     {
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                } 
                push(NUMBER_VAL(-AS_NUMBER(pop())));
                break;
            }
            case OP_RETURN: {
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }

    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}

InterpretResult interpret(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

InterpretResult runBytecode(const char *source) {
    // TODO: convert bytecode to chunk
    Chunk chunk;
    initChunk(&chunk);

    size_t i = 0;
    double constants[] = { 1.0, 1.0 };
    for (;source[i] != (char) 0xc0 
        && source[i + 1] != (char)0xff 
        && source[i + 2] != (char)0xee; i++);
    i += 3;

    for (;source[i - 1] != OP_RETURN; i++) 
        switch (source[i]) {
            case OP_CONSTANT:       writeConstant(&chunk, NUMBER_VAL(constants[source[++i]]), 0); break;
            default:                writeChunk(&chunk, (uint8_t) source[i], 0);                 
        }

    disassembleChunk(&chunk, "test chunk");
    InterpretResult result = interpret(&chunk);
    freeChunk(&chunk);
    return result;
}

static char *readFile(const char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Could not open file '%s'.\n", path);
        exit(74);
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *) malloc(fileSize + 1);
    
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read file '%s'.\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file '%s'.\n", path);
        exit(74);
    }
    
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char *path) {
    char *source = readFile(path);
    InterpretResult result = runBytecode(source);
    free(source); 

    if (result == INTERPRET_COMPILE_ERROR) 
        exit(65);

    if (result == INTERPRET_RUNTIME_ERROR) 
        exit(70);
}

int main(int argc, char **argv) {
    initVM();
    runFile("main.nco");
    // runBytecode("");
    freeVM();
    return 0;
}
