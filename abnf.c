#include <stdio.h>
#include <stdlib.h>

#include "abnf.h"

void
alpha(State *st)
{
	CHOOSE(
		OR(ASCIIRANGE(0x41, 0x5a))
		OR(ASCIIRANGE(0x61, 0x7a))
	)
}

void
bit(State *st)
{
	ASCIIRANGE(0x30, 0x31)
}

void
crlf(State *st)
{
	STR(\r\n)
}

void
digit(State *st)
{
	ASCIIRANGE(0x30, 0x39)
}

void
dquote(State *st)
{
	ASCIIRANGE(0x22, 0x22)
}

void
hexdig(State *st)
{
	CHOOSE(
		OR(RULENAME(digit))
		OR(STR(A))
		OR(STR(B))
		OR(STR(C))
		OR(STR(D))
		OR(STR(E))
		OR(STR(F))
	)
}

void
htab(State *st)
{
	ASCIIRANGE(0x09, 0x09)
}

void
lpar(State *st)
{
	ASCIIRANGE(0x28, 0x28)
}

void
lwsp(State *st)
{
	REPEAT(0, INF,
		CHOOSE(
			OR(RULENAME(wsp))
			OR(
				EVERY(
					AND(RULENAME(crlf))
					AND(RULENAME(wsp))
				)
			)
		)
	)
}

Node *
node_new(void)
{
	Node *node;
	if ((node = malloc(sizeof(Node))) == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	node->left = NULL;
	node->right = NULL;
	node->tag = NULL;
	node->value = NULL;
	node->taglen = 0;
	node->valuelen = 0;
	return node;
}

void
node_free(Node *node)
{
	if (node == NULL) /* allows node_free(node) when node == NULL */
		return;
	if (node->right != NULL)
		node_free(node->right);
	if (node->left != NULL)
		node_free(node->left);
	free(node);
	node = NULL;
}

void
node_fill(Node *node, char *value, int valuelen, char *tag, int taglen)
{
	node->value = value;
	node->valuelen = valuelen;
	node->tag = tag;
	node->taglen = taglen;
}

void
octet(State *st)
{
	/* omits 0x00 as it's also C string cookie */
	ASCIIRANGE(0x01, 0xff)
}

void
printtree(Node *tree, int depth) {
	int i;

	if (tree == NULL)
		return;
	for (i = 0; i < depth; i++)
		putchar('\t');
	for (i = 0; i < tree->taglen; i++)
		putchar(tree->tag[i]);
	putchar(':');
	putchar(' ');
	putchar('"');
	for (i = 0; i < tree->valuelen; i++)
		putchar(tree->value[i]);
	putchar('"');
	putchar('\n');
	if (tree->right != NULL)
		printtree(tree->right, depth + 1);
	if (tree->left != NULL)
		printtree(tree->left, depth);
}

void
rpar(State *st)
{
	ASCIIRANGE(0x29, 0x29)
}

void
sp(State *st)
{
	ASCIIRANGE(0x20, 0x20)
}

void
vchar(State *st)
{
	ASCIIRANGE(0x21, 0x7e)
}

void
wsp(State *st)
{
	CHOOSE(
		OR(RULENAME(sp))
		OR(RULENAME(htab))
	)
}

Node *tree;
