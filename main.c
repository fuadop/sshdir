#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sshconfig/parser.h"

int main() {
	StringList p = expand_path("~/.ssh/config");
	FILE *f = fopen(p.list[0], "rb");

	Config c = parse(f);

	int fd[2];
	if (pipe(fd) != 0) {
		puts("pipe error");
		exit(1);
	}

	for (int i = 0; i < c.len; i++) {
		Rule r = c.rules[i];

		char* host = "-";
		char* user = "-";
		char* hostname = "-";

		for (int j = 0; j < r.len; j++) {
			Attribute a = r.attributes[j];

			if (strcmp(a.key, "Host") == 0) {
				host = a.value;
				continue;
			}

			if (strcmp(a.key, "User") == 0) {
				user = a.value;
				continue;
			}

			if (strcmp(a.key, "HostName") == 0) {
				hostname = a.value;
				continue;
			}
		}

		char *msg = malloc(sizeof(char) * (strlen(host) + strlen(user) + strlen(hostname) + 1));
		memset(msg, 0, sizeof(char) * (strlen(host) + strlen(user) + strlen(hostname) + 1));

		sprintf(msg, "%-40s: %s@%s\n", host, user, hostname);
		write(fd[1], msg, strlen(msg) + 1);

		free(msg);
	}

	// cleanup memory
	fclose(f);
	free_config(c);
	free(p.list[0]);
	free(p.list);

	// handover to fzf
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);

	char *args[] = {
		"fzf",
		"--delimiter",
		":",
		"--bind",
		"enter:become(ssh {1})",
		NULL
	};

	execvp(args[0], args);

	puts("error opening fzf");

	return 1;
}
