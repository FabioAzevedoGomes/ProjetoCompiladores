# Etapa 4

## error_t contents:
  
  - `UNDECLARED`
    * `data1 : char*` : Name of undeclared id

  - `DECLARED`
    * `data1 : symbol_t *`: Previous declaration
    * `data2 : symbol_t *`: Current declaration

  - `FUNCTION` and `VARIABLE` and `VECTOR`
    * `data1 : symbol_t*` : Symbol information
    * `data2 : node_t *` : Node containing incorrect usage

  - `WRONG_TYPE`
    * `data1 : node_t *` : Node containing incorreect usage

  - `CHAR_TO_X` and `STRING_TO_X`
    * `data1 : node_t *` : Node containing usage of char or string

  - `STRING_SIZE`
    * `data1 : symbol_t *` : String symbol information
    * `data2 : node_t *` : Node containing excessive size
    * `data3 : int *` : Invalid size

  - `EXCESS_ARGS` and  `MISSING_ARGS`
    * `data1 : symbol_t *` : Function symbol information
    * `data2 : node_t *` : Node containing the function call

  - `WRONG_TYPE_ARGS`
    * `data1 : symbol_t *` : Function symbol information
    * `data2 : node_t *` : Node containing the function call
    * `data3 : int *` : Index of the incorrect argument

  - `WRONG_PAR_INPUT`
    * `data1 : node_t *` : Node containing operand being input to

  - `WRONG_PAR_OUTPUT`
    * `data1 : node_t *` : Node containing expression being output

  - `WRONG_PAR_RETURN`
    * `data1 : symbol_t *` : Function symbol information
    * `data2 : node_t *`   : Node containing returned expression
  
  - `WRONG_PAR_SHIFT`
    * `data1 : int *` : Value being used for shift indexing