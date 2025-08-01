#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *log_file = NULL;

void init_log() {

	log_file = fopen("", "a");
	if (!log_file) {
		perror("Failed to open log file");
		exit(1);
	}

	time_t now = time(NULL);
	fprintf(log_file, "\n--- Log started at: %s", ctime(&now));
	fflush(log_file);
}


void log_printf(const char *format, ...) {
	va_list args;

	// Print to stdout
	va_start(args, format);
	vprintf(format, args);
	va_end(args);

	// Print to log
	va_start(args, format);
	vfprintf(log_file, format, args);
	va_end(args);

	fflush(log_file);
}


void log_fgets(char *buffer, size_t size, FILE *stream) {
	if (fgets(buffer, size, stream) != NULL) {
		log_printf("[User input] %s", buffer);
	}
}

