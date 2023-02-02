#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	char* buffer;
	size_t buffer_length; // size_tって何やっけ？
	ssize_t input_length; // ssize_tって何やっけ？
} InputBuffer;

// メタコマンドの解析結果
typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

// SQL文の解析結果
typedef enum {
	PREPARE_SUCCESS,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// SQL文の種別
typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

// SQL文の構造
typedef struct {
	StatementType type;
} Statement;

InputBuffer* new_input_buffer() {
	InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;

	return input_buffer;
}

void print_prompt() {
	printf("db > ");
}

void read_input(InputBuffer* input_buffer) {
	// getlineのリファレンス
	// https://kazmax.zpp.jp/cmd/g/getline.3.html
	// 第1引数:入力した文字が改行ありで入る
	// 第2引数:bufferに割り当てられたサイズが入る
	// getlineの中でメモリ割当されている
	ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
	if(bytes_read <= 0) {
		printf("Error reading input\n");
		exit(EXIT_FAILURE);
	}

	// getlineは改行を含むため、改行を削除している
	// for debug
	//printf("%s", input_buffer->buffer);
	//printf("buffer_length: %ld\n", input_buffer->buffer_length);
	//printf("bytes_read: %ld\n", bytes_read);
	//
	input_buffer->input_length = bytes_read - 1;
	input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer) {
	free(input_buffer->buffer);
	free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
	if(strcmp(input_buffer->buffer, ".exit") == 0) {
		exit(EXIT_FAILURE);
	} else {
		return META_COMMAND_UNRECOGNIZED_COMMAND;
	}
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
	// insertの場合は後ろにデータが続くため、strncmpを使っている
	// (e.g. insert 1 cstack foo@bar.com)
	// selectも同じなのでは？
	if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
		statement->type = STATEMENT_INSERT;
		return PREPARE_SUCCESS;
	}
	if(strcmp(input_buffer->buffer, "select") == 0) {
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}
	return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
	switch(statement->type) {
		case(STATEMENT_INSERT):
			printf("This is where we would do an insert\n");
			break;
		case(STATEMENT_SELECT):
			printf("This is where we would do an select\n");
			break;
	}
}

int main(int argc, char* argv[]) {
	InputBuffer* input_buffer = new_input_buffer();
	while(true) {
		print_prompt();
		read_input(input_buffer);

		// SQLiteのメタコマンドかどうかを確認する
		// 先頭が"."であれば、メタコマンド
		if(input_buffer->buffer[0] == '.') {
			switch(do_meta_command(input_buffer)) {
				case(META_COMMAND_SUCCESS):
					continue;
				case(META_COMMAND_UNRECOGNIZED_COMMAND):
					printf("Unrecognized command '%s'\n", input_buffer->buffer);
					continue;
			}
		}

		// 入力されたSQLを解析する
		Statement statement;
		switch(prepare_statement(input_buffer, &statement)) {
			case(PREPARE_SUCCESS):
				break;
			case(PREPARE_UNRECOGNIZED_STATEMENT):
				printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
				continue;
		}

		// SQL実行
		execute_statement(&statement);
		printf("Executed.\n");
	}
}
