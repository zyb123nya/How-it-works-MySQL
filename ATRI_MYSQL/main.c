#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//定义缓冲区(I/O)
typedef struct {
	char *buffer;
	size_t buffer_length;
	int input_length;
} InputBuffer;
//定义一下测试指令
typedef enum{
	META_COMMAND_SUCCESS,
	META_COMMAND_NOT_COMMAND
} MetaCommandResult;
//定义一些console输出
typedef enum{
	PREPARE_SUCCESS,
	PREPARE_NOT_STATEMENT
}PrepareResult;
//定义几个操作类型和状态
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;
typedef struct {
  StatementType type;
} Statement;

//支持chinese字符
void windows_cmd_support_utf8(void)
{
#ifdef WIN32
    system("chcp 65001 & cls"); //cls 用来清除 chcp 的输出
#endif
}
//创建缓冲区
InputBuffer *new_input_buffer() {
	InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;
	return input_buffer;
}
//测试指令的一个容器罢了
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
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
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_NOT_STATEMENT;
}
//命令行 ATRI可爱捏（
void print_prompt() {
	printf("ATRI db> ");
}
//执行与操作状态
void execute_statement(Statement* statement) {
  switch (statement->type) {
	//insert操作
    case (STATEMENT_INSERT):
      printf(" insert操作开始啦，看看这次又有什么新的数据!.\n");
      break;
	//select操作
    case (STATEMENT_SELECT):
      printf("select操作，主人要什么数据nya?一般格式为select * from 表名哦~\n");
      break;
  }
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

int main() {
	windows_cmd_support_utf8();
	InputBuffer *input_buffer = new_input_buffer();
	while (1) {
		//>db
		print_prompt();
		//输入
		read_input(input_buffer);
		//测试.exit指令
			if(input_buffer->buffer[0]=='.'){
				switch(do_meta_command(input_buffer)){
					case (META_COMMAND_SUCCESS):
						continue;
					case(META_COMMAND_NOT_COMMAND):
						printf("Unrecognized command '%s'\n", input_buffer->buffer);
						continue;
				}
			}
		//其他指令
		Statement statement;
		switch(prepare_statement(input_buffer,&statement)){
			case(PREPARE_SUCCESS):
				break;
			case(PREPARE_NOT_STATEMENT):
				printf("没有找到相关关键词...要不要再看看指令?'%s'.\n",input_buffer->buffer);
				continue;
			}
		execute_statement(&statement);
		printf("执行成功nya!\n");
		}
	
}