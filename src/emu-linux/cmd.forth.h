
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

//
// No thought put into this yet, just what the AI gave me from a google search.
//
// I just wanted to have something basic here to encourage myself to take this
// further.  This is very limited, it can compile a word, but not use that
// word as part of future compiled word.  No return stack for one.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 256
#define DICT_SIZE  512
#define FORTH_MEM_SIZE   4096

// Forth execution states
typedef enum { STATE_INTERPRET, STATE_COMPILE } State;

// Bytecode tokens for primitives
typedef enum {
    OP_EXIT, OP_LIT, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_DUP, OP_DROP, OP_EMIT, OP_DOT, OP_DOTX
} OpCode;

// Dictionary entry structure
typedef struct {
    char name[32];
    int is_primitive;
    union {
        OpCode prim;       // For native primitives
        int mem_addr;      // For compiled code in array
    } code;
} Word;

// Global virtual machine state
int stack[STACK_SIZE];
int sp = 0;

Word dict[DICT_SIZE];
int dict_p = 0;

unsigned char bytecode[ FORTH_MEM_SIZE ];
int here = 0;

State state = STATE_INTERPRET;
int current_compile_dict_idx = -1;

// Stack operations
void push(int val) {
  if ( sp >= STACK_SIZE ) { println( "Stack Overflow" ); return; }
  stack[sp++] = val;
}

int pop() {
    if (sp <= 0) { println( "Stack Underflow" ); return NULL; }
    return stack[--sp];
}

// Find a word in the dictionary
int find_word( const char *name ) {
    for (int i = dict_p - 1; i >= 0; i--) {
        if ( strcmp(dict[i].name, name ) == 0 ) return i;
    }
    return -1;
}

// Register primitive native words
void add_primitive(const char *name, OpCode op) {
    strcpy(dict[dict_p].name, name);
    dict[dict_p].is_primitive = 1;
    dict[dict_p].code.prim = op;
    dict_p++;
}

// Inner Interpreter: Executes compiled bytecode
void execute_bytecode(int addr) {
    int ip = addr;
    while (1) {
        OpCode op = (OpCode)bytecode[ip++];
        switch (op) {
            case OP_EXIT: return;
            case OP_LIT: {
                int val = 0;
                // Read 4-byte integer from stream
                memcpy(&val, &bytecode[ip], sizeof(int));
                ip += sizeof(int);
                push(val);
                break;
            }
            case OP_ADD:  push(pop() + pop()); break;
            case OP_SUB:  { int b = pop(); int a = pop(); push(a - b); break; }
            case OP_MUL:  push(pop() * pop()); break;
            case OP_DIV:  { int b = pop(); int a = pop(); push(a / b); break; }
            case OP_DUP:  { int val = pop(); push(val); push(val); break; }
            case OP_DROP: pop(); break;
            case OP_EMIT: printch( pop() ); break;
            case OP_DOT:  print( pop() ); break;
        }
    }
}

// Outer Interpreter / Compiler Loop
void process_token(char *token) {
    if (token == NULL || strlen(token) == 0) return;

    if (state == STATE_COMPILE) {
        // Handle compilation mode termination
        if (strcmp(token, ";") == 0) {
            bytecode[here++] = OP_EXIT; // Compile exit sequence
            state = STATE_INTERPRET;
            return;
        }

        int idx = find_word(token);
        if (idx != -1) {
            if (dict[idx].is_primitive) {
                bytecode[here++] = (unsigned char)dict[idx].code.prim;
            } else {
                // Threaded code call sequence
                bytecode[here++] = OP_LIT; // Target memory pointer address
                int addr = dict[idx].code.mem_addr;
                memcpy(&bytecode[here], &addr, sizeof(int));
                here += sizeof(int);
                // In a true indirect/direct system we leap; here we evaluate via inline expansions
                // For this minimal engine, we can trigger nesting by keeping it direct
            }
        } else {
            // Assume Token is a raw literal number to compile
            char *end;
            int val = pnum( token, 16 );
            //println( val );
            //println( sizeof( int ) );
            //if (*end == '\0') {
                bytecode[here++] = OP_LIT;
                memcpy(&bytecode[here], &val, sizeof(int));
                here += sizeof(int);
            //} else {
                //print( "Error: Unknown token during compilation: " );
                //println( token );
                //state = STATE_INTERPRET; // Abort
            //}
        }
    } else {
        // Handle Interpret Mode execution
        if (strcmp(token, ":") == 0) {
            // Transition into Colon Definition Compiler Mode
            char *name = strtok(NULL, " \t\r\n");
            if (!name) return;
            strcpy(dict[dict_p].name, name);
            dict[dict_p].is_primitive = 0;
            dict[dict_p].code.mem_addr = here;
            current_compile_dict_idx = dict_p;
            dict_p++;
            state = STATE_COMPILE;
            return;
        }

        int idx = find_word(token);
        if (idx != -1) {
            if (dict[idx].is_primitive) {
                // Execute core primitives directly in interpreting mode
                switch (dict[idx].code.prim) {
                    case OP_ADD:  push(pop() + pop()); break;
                    case OP_SUB:  { int b = pop(); int a = pop(); push(a - b); break; }
                    case OP_MUL:  push(pop() * pop()); break;
                    case OP_DIV:  { int b = pop(); int a = pop(); push(a / b); break; }
                    case OP_DUP:  { int val = pop(); push(val); push(val); break; }
                    case OP_DROP: pop(); break;
                    case OP_EMIT: printch( (char)pop() ); break;
                    case OP_DOT:  print( pop() ); break;
                    case OP_DOTX: print( hex0( pop(), 0 ) );
                    default: break;
                }
            } else {
                // Run compiled user code definitions
                execute_bytecode(dict[idx].code.mem_addr);
            }
        } else {
            // Evaluate interpreted integer numbers
            char *end;
            int val = pnum( token, 16 );
            //if ( val == '\0') {
                push(val);
            //} else {
                //print( "Error: " );
                //print( token );
                //println( " ?" );
            //}
        }
    }
}




//
// cli commands, not in forth environment
//

bool forth_init_run = false;

static void forth_init() {
  // Initialize standard primitives inside the baseline dictionary
    add_primitive( "+",    OP_ADD );
    add_primitive( "-",    OP_SUB );
    add_primitive( "*",    OP_MUL );
    add_primitive( "/",    OP_DIV );
    add_primitive( "dup",  OP_DUP );
    add_primitive( "drop", OP_DROP );
    add_primitive( "emit", OP_EMIT );
    add_primitive( ".",    OP_DOT );
    add_primitive( ".x",    OP_DOTX );

    println("Minimal C-Forth Initialized. Type code below (e.g., 2 3 + .)");
    forth_init_run = true;
}

static void forth() {
  if ( ! forth_init_run ) forth_init();
  if ( cmdline.plen > 0 ) {
    //println( cmdline.cmd );
    char *token = strtok( cmdline.cmd, " " );
    if ( ( strcmp( token, "forth" ) == 0 ) || ( strcmp( token, "calc" ) == 0 ) ) {
      token = strtok( NULL, " " );  // eat forth
      if ( token == NULL ) {
        cmd_mode = ! cmd_mode;
        return;
      }
    }
    int i = 0;
    while ( i++ < cmdline.plen ) {
      //print( i );
      //print( " " );
      //println( token );
      process_token( token );
      token = strtok( NULL, " " );
    }
    if (state == STATE_INTERPRET) println(" ok");
  }
}

cmd_entry_t cmds_forth[] = {
  { "calc", forth, "", "a work in progress" },
  { "forth", forth, "", "a work in progress" },
  { NULL, NULL, NULL, NULL }
};
