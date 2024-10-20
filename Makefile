bin/sshdir: main.c sshconfig/parser.c
	mkdir -p ./bin
	gcc -I./ -o ./bin/sshdir main.c sshconfig/parser.c

.PHONY: install
install: bin/sshdir
	sudo cp ./bin/sshdir /usr/local/bin/sshdir

.PHONY: clean
clean:
	rm -f ./bin/sshdir
