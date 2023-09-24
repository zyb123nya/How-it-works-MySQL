#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
	char *buffer;
	size_t buffer_length;
	int input_length;
} InputBuffer;

//创建缓冲区
InputBuffer *new_input_buffer() {
	InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;

	return input_buffer;
}

void print_prompt() {
	printf("db > ");
}
// 读取用户输入
void read_input(InputBuffer *input_buffer) {
	int bytes_read =
	    getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

	if (bytes_read <= 0) {
		printf("Error reading input\n");
		exit(EXIT_FAILURE);
	}

	// Ignore trailing newline
	input_buffer->input_length = bytes_read - 1;
	input_buffer->buffer[bytes_read - 1] = 0;
}
//关闭缓冲区
void close_input_buffer(InputBuffer *input_buffer) {
	free(input_buffer->buffer);
	free(input_buffer);
}

int main() {
	InputBuffer *input_buffer = new_input_buffer();
	while (1) {
		//>db
		print_prompt();
		//输入
		read_input(input_buffer);
		//测试.exit指令与其他
		if (strcmp(input_buffer->buffer, ".exit") == 0) {
			close_input_buffer(input_buffer);
			exit(EXIT_SUCCESS);
		} else {
			printf("Unrecognized command '%s'.\n", input_buffer->buffer);
		}
	}
}