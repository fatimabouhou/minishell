CC     = gcc
CFLAGS = -Wall -Wextra -g

# ─── Test 1 : parser seul ───────────────────────────────────────
test_parser: test_parser.c parser.c parser.h
	$(CC) $(CFLAGS) -o test_parser test_parser.c parser.c
	@echo "✓ test_parser compilé → lance avec : ./test_parser"

# ─── Test 2 : executor + parser (sans binôme) ───────────────────
test_executor: test_executor.c parser.c executor.c parser.h executor.h
	$(CC) $(CFLAGS) -o test_executor test_executor.c parser.c executor.c
	@echo "✓ test_executor compilé → lance avec : ./test_executor"

# ─── Projet final (quand binôme a fini) ─────────────────────────
SRCS = main.c parser.c executor.c builtins.c signals.c utils.c
OBJS = $(SRCS:.c=.o)

myshell: $(OBJS)
	$(CC) $(CFLAGS) -o myshell $(OBJS)
	@echo "✓ myshell compilé → lance avec : ./myshell"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Nettoyage ───────────────────────────────────────────────────
clean:
	rm -f $(OBJS) myshell test_parser test_executor

.PHONY: clean