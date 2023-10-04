#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdint.h>

//定义缓冲区(I/O)
typedef struct {
	char *buffer;
	size_t buffer_length;
	int input_length;
} InputBuffer;
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;
//定义一下测试指令
typedef enum{
	META_COMMAND_SUCCESS,
	META_COMMAND_NOT_COMMAND
} MetaCommandResult;
//定义一些console输出
typedef enum{
	PREPARE_SUCCESS,
	PREPARE_SYNTAX_ERROR,
	PREPARE_NOT_STATEMENT
}PrepareResult;

//定义几个操作类型和状态
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
  int id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;
// #define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
const uint32_t ID_OFFSET = 0;
const uint32_t ID_SIZE = 255;
const uint32_t USERNAME_SIZE = 255;
const uint32_t EMAIL_SIZE = 255;
const uint32_t USERNAME_OFFSET = 255;
const uint32_t EMAIL_OFFSET = 255;
const uint32_t ROW_SIZE = 255;
#define TABLE_MAX_PAGES 100
const uint32_t PAGE_SIZE = 4096;

const uint32_t ROWS_PER_PAGE = 4096 / 255;
const uint32_t TABLE_MAX_ROWS = 16 * TABLE_MAX_PAGES;
typedef struct {
  StatementType type;
  Row row_to_insert;
} Statement;

//table结构体
typedef struct {
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
} Table;
void print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
//序列化 对象转字节序列
void serialize_row(Row* source, void* destination) {
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
 memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}
// 字节序列转对象
void deserialize_row(void* source, Row* destination) {
 memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
 memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
 memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_num) {
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void* page = table->pages[page_num];
  if (page == NULL) {
    page = table->pages[page_num] = malloc(PAGE_SIZE);
  }
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}
Table* new_table() {
  Table* table = (Table*)malloc(sizeof(Table));
  table->num_rows = 0;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
     table->pages[i] = NULL;
  }
  return table;
}
void free_table(Table* table) {
    for (int i = 0; table->pages[i]; i++) {
	free(table->pages[i]);
    }
    free(table);
}

//创建缓冲区
InputBuffer *new_input_buffer() {
	InputBuffer *input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;
	return input_buffer;
}
// 读取用户输入
void read_input(InputBuffer *input_buffer) {
	int bytes_read =
	    getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

	if (bytes_read <= 0) {
		printf("Error reading input\n");
		exit(EXIT_FAILURE);
	}

	//在缓冲区忽略"/n"之类的换行符
	input_buffer->input_length = bytes_read - 1;
	input_buffer->buffer[bytes_read - 1] = 0;
}
//关闭缓冲区
void close_input_buffer(InputBuffer *input_buffer) {
	free(input_buffer->buffer);
	free(input_buffer);
}

//测试指令的一个容器罢了
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table *table) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    free_table(table);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_NOT_COMMAND;
  }
}
//sql编译器，也就是各种指令
PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
	int args_assigned = sscanf(input_buffer ->buffer,"insert %d %s %s",&(statement->row_to_insert.id),statement->row_to_insert.username, statement->row_to_insert.email);
	if (args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_NOT_STATEMENT;
}
ExecuteResult execute_insert(Statement* statement, Table* table) {
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row_to_insert = &(statement->row_to_insert);

  serialize_row(row_to_insert, row_slot(table, table->num_rows));
  table->num_rows += 1;

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    deserialize_row(row_slot(table, i), &row);
    print_row(&row);
  }
  return EXECUTE_SUCCESS;
}
//执行与操作状态
ExecuteResult execute_statement(Statement* statement, Table* table) {
  switch (statement->type) {
	//insert操作
    case (STATEMENT_INSERT):
		return execute_insert(statement, table);
	//select操作
    case (STATEMENT_SELECT):
		return execute_select(statement, table);
  }
}

//支持chinese字符
void windows_cmd_support_utf8(void)
{
#ifdef WIN32
    system("chcp 65001 & cls"); //cls 用来清除 chcp 的输出
#endif
}
//命令行 ATRI可爱捏（
void print_prompt() {
	printf("ATRI db> ");
}
int main() {
	Table* table = new_table();
	windows_cmd_support_utf8();
	InputBuffer *input_buffer = new_input_buffer();
	while (1) {
		//>db
		print_prompt();
		//输入
		read_input(input_buffer);
		//测试.exit指令
			if(input_buffer->buffer[0]=='.'){
				switch(do_meta_command(input_buffer, table)){
					case (META_COMMAND_SUCCESS):
						continue;
					case(META_COMMAND_NOT_COMMAND):
						printf("不认识哦..,是不是想输入.exit呢?'%s'\n", input_buffer->buffer);
						continue;
				}
			}
		//其他指令
		Statement statement;
		switch(prepare_statement(input_buffer,&statement)){
			case(PREPARE_SUCCESS):
				break;
   			case (PREPARE_SYNTAX_ERROR):
       			printf("Syntax error. Could not parse statement.\n");
       			continue;
			case(PREPARE_NOT_STATEMENT):
				printf("没有找到相关关键词...要不要再看看指令?'%s'.\n",input_buffer->buffer);
				continue;
			}
		switch (execute_statement(&statement, table)) {
			case (EXECUTE_SUCCESS):
				printf("Executed.\n");
				break;
			case (EXECUTE_TABLE_FULL):
				printf("Error: Table full.\n");
				break;
			}
	}
	
}