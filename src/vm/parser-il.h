#ifndef _PARSER_IL_H_
#define _PARSER_IL_H_

/**
 * @brief read an unsigned integer value from string
 * starting @ position start
 * @param line
 * @return number read (positive value) or error (negative value)
 */
int extract_number(const char * line);

/**
 * @brief extract bit and byte from X/Y expression starting @ start
 * @param the string
 * @param the byte
 * @param the bit
 * @return OK or error
 */
int extract_arguments(const char* buf,  
                     BYTE* byte, 
                     BYTE* bit);

/**
 * @brief read optional descriptor (currently only f) 
 * from line at index
 * and parse grammatically
 * @param line
 * @param current operand
 * @param c index
 * @return IL operand symbol
 */
BYTE read_type(const char *line, 
                BYTE *operand,                 
                unsigned int index);


/**
 * @brief read operand  
 * from line at index
 * and parse grammatically
 * @param line
 * @param c index
 * @return IL operand symbol
 */
BYTE read_operand(const char * line, unsigned int index);

/**
 * @brief read up to ';' or /n
 * @param the line to trunkate comments from
 */
void read_line_trunk_comments(char * line);

/**
 * @brief find last ':', truncate up to there, store label.
 * @param buf a place to store labelless lines
 * @param label_buf the extracted label
 */
void trunk_label(const char * line, 
                 char * buf, 
                 char * label_buf);
                 
/**
 * @brief trunkate all whitespaces left and right
 * @param the line to trunkate whitespaces from
 */
void trunk_whitespace(char* line);                 

/**
 * @brief find first ' ','!','('. store modifier (0 if not found)
 * @param buf a string
 * @param pos points to where was the modifier found
 * @return the modifier
 */
BYTE read_modifier(const char * buf, char ** pos);

/**
 * @brief read operator from beginning to modifier, check if invalid. 
 * @param buf a string
 * @param stop points to end of operator
 * @return error code if invalid, otherwise the operator
 */
BYTE read_operator(const char * buf, const char * stop);

/**
 * @brief check modifier for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int check_modifier(const instruction_t op);

/**
 * @brief check operand for correctness
 * @param an instruction
 * @return error code if not correct, 0 otherwise
 */
int check_operand(instruction_t op);

/**
 *@brief find arguments
 * @param a line
 * @param operand
 * @param byte
 * @param bit
 * @return error code on failure
 */
int find_arguments(const char* buf,   
                   BYTE* operand, 
                   BYTE* byte,
                   BYTE* bit);

/**
 * @brief parse IL line and generate microcode
 * line format:[label:]<operator>[<modifier>[%<operand><byte>[/<bit>]]|<label>][;comment]
 * @param a line of IL code
 * @param the rung to generate micorcode in
 * @return OK or error
 */
int parse_il_line(char * line, rung_t r);
 
/**
  * @brief parse IL program
  * @param a unique program identifier
  * @param the program as an allocated buffer of allocated strings
  * @param the plc to store the generated microcode to
  * @return plc with updated status  
  */
plc_t parse_il_program(const char* name, 
                       const char lines[][MAXSTR], 
                       plc_t p);


#endif //_PARSER-IL_H
