#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCH "fb001"

// Define the CPU structure
typedef struct {
    int pc;  // Program Counter
    int acc; // Accumulator
    int ax;
    int bx;
    int cx;
    int flags; // Flag Register
    int ip; // Instruction Pointer
    int segment; // Segment Register
    int cpu_error_flag; // CPU Error Flag
    int ram_error_flag; // RAM Error Flag
    int gpu_error_flag; // GPU Error Flag
    int disk_error_flag; // Disk Error Flag
} CPU;

// Define the Label structure
typedef struct {
    char name[32];
    int address;
} Label;

// Function to set the error flags
void set_error_flags(CPU* cpu, int cpu_error, int ram_error, int gpu_error, int disk_error) {
    if (cpu_error != 0) {
        cpu->cpu_error_flag = cpu_error;
    } else if (ram_error != 0) {
        cpu->ram_error_flag = ram_error;
    } else if (gpu_error != 0) {
        cpu->gpu_error_flag = gpu_error;
    } else if (disk_error != 0) {
        cpu->disk_error_flag = disk_error;
    }
}

// Function to execute an instruction
void executeInstruction(CPU* cpu, int opcode, int operand, Label* labelTable, int numLabels) {
    // Execute the instruction
    switch (opcode) {
        case 0x01: // Load instruction
            cpu->acc = operand;
            break;
        case 0x02: // Add instruction
            cpu->acc += operand;
            break;
        case 0x03: // Subtract instruction
            cpu->acc -= operand;
            break;
        case 0x04: // Jump instruction
            if (operand >= 0 && operand < numLabels) {
                cpu->pc = labelTable[operand].address; // Jump to the address of the label
            } else {
                printf("Invalid label operand\n");
            }
            break;
        case 0x05: // Print instruction
            if (operand == 0x00) {
                printf("Accumulator: %d\n", cpu->acc);
            } else if (operand == 0x001) {
                printf("Register 1: %d\n", cpu->ax);
            } else {
                printf("Unknown register");
            }
            break;
        case 0x06: // Label instruction
            // Do nothing, as this is just a marker for labels
            break;
        default:
            printf("Unknown instruction\n");
            break;
    }
}

void cpu_init(CPU* cpu) {
    cpu->pc = 0;
    cpu->acc = 0;
    cpu->ax = 0;
    cpu->bx = 0;
    cpu->cx = 0;
    cpu->flags = 0;
    cpu->ip = 0;
    cpu->segment = 0;
    cpu->cpu_error_flag = 0;
    cpu->ram_error_flag = 0;
    cpu->gpu_error_flag = 0;
    cpu->disk_error_flag = 0;
}

void cpu_stage2(CPU* cpu, int program[], int numInstructions, Label* labelTable, int numLabels) {
    for (int i = 0; i < numInstructions; i++) {
        int instruction = program[i];
        int opcode = (instruction >> 24) & 0xFF;
        int operand = instruction & 0xFFFFFF;

        // Check if the opcode is for a label
        if (opcode == 0x06) {
            // Find the corresponding label in the label table
            for (int j = 0; j < numLabels; j++) {
                if (labelTable[j].address == operand) {
                    printf("Label: %s\n", labelTable[j].name);
                    break;
                }
            }
        } else {
            // Execute the instruction
            executeInstruction(cpu, opcode, operand);
        }
    }
}

// Function to add a label to the labelTable
void addLabel(Label** labelTable, int* numLabels, const char* name, int address) {
    // Reallocate memory for the labelTable
    *labelTable = (Label*)realloc(*labelTable, (*numLabels + 1) * sizeof(Label));

    // Copy the name and address to the new label entry
    strcpy((*labelTable)[*numLabels].name, name);
    (*labelTable)[*numLabels].address = address;

    // Increment the number of labels
    (*numLabels)++;
}

int main() {
    // Initialize the CPU
    CPU cpu;
    cpu_init(&cpu);

    // Program memory
    int program[] = {
        0x0100000A, // Load 10 into the accumulator
        0x02000005, // Add 5 to the accumulator
        0x03000003, // Subtract 3 from the accumulator
        0x04000001, // Jump to instruction at address 1
        0x05000001, // Print the value in register 1
        0x06AAAAAA, // Label with hex code 0x06 and address 0xAAAAAA
        0x06BBBBBB,
        0x0100000F, // Load 15 into the accumulator
        0x02000003, // Add 3 to the accumulator
        0x05000000  // Print the accumulator value
    };

    int numInstructions = sizeof(program) / sizeof(program[0]);

    // Define the label table dynamically
    Label* labelTable = NULL;
    int numLabels = 0;

        // Add labels to the label table
    addLabel(&labelTable, &numLabels, "loop", 0xAAAAAA);
    addLabel(&labelTable, &numLabels, "End", 0xBBBBBB);

    // Execute the program
    cpu_stage2(&cpu, program, numInstructions, labelTable, numLabels);

    // Free dynamically allocated memory for the label table
    free(labelTable);

    return 0;
}
