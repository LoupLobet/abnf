#ifndef _ABNF_H_
#define _ABNF_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct Node {
	struct Node *left;
	struct Node *right;
	char *tag;
	char *value;
	int taglen;
	int valuelen;
} Node;

typedef struct {
	Node *node;
	Node *left;
	char *current;
	int status;
} State;

enum { FAIL, MATCH };

/* "infinity", see REPEAT macro */
#define INF -1

#define ASCIIRANGE(n, m)\
do {\
	if (n <= (unsigned char) *(st->current) && (unsigned char) *(st->current) <= m) {\
		st->status = MATCH;\
		st->node = node_new();\
		node_fill(st->node, st->current, 1, "ascii", 6);\
		st->left = st->node;\
		st->current++;\
	} else {\
		st->node = NULL;\
		st->status = FAIL;\
	}\
} while(0);\

#define STR(s) \
do {\
	int i;\
	int fail;\
\
	fail = 0;\
	for (i = 0; i < strlen(#s); i++) {\
		if (tolower(#s[i]) != tolower(st->current[i])) {\
			st->status = FAIL;\
			fail = 1;\
			break;\
		}\
	}\
	if (fail) {\
		st->node = NULL;\
		break;\
	}\
	st->status = MATCH;\
	st->node = node_new();\
	node_fill(st->node, st->current, i, "string", 6);\
	st->left = st->node;\
	st->current += i;\
} while(0);\


#define CHOOSE(body) \
do {\
	body\
	node_free(st->node);\
	st->status = FAIL;\
} while(0);\

#define OR(body) \
	body\
	if (st->status == MATCH) {\
		break;\
	}\

#define EVERY(body) \
do {\
	Node *nanchor;\
	Node *node;\
	char *sanchor;\
	int root;\
\
	sanchor = st->current;\
	nanchor = NULL;\
	node = NULL;\
	root = 1;\
	body\
	st->node = nanchor;\
	st->left = node;\
	st->status = MATCH;\
} while(0);\

#define AND(body) \
	body\
\
	if (st->status == FAIL) {\
		st->current = sanchor;\
		node_free(nanchor);\
		node_free(st->node);\
		break;\
	}\
	/* MATCH case */ \
	if (st->node != NULL) {\
		if (root) { /* set the node anchor */ \
			nanchor = st->node;\
			node = st->left;\
			root = 0;\
		} else {\
			node->left = st->node;\
			node = st->left /*node->left*/;\
		}\
	}\

#define OPTIONAL(body) \
do {\
	body\
	st->status = MATCH;\
} while(0);\

#define RULENAME(func) \
do {\
	Node *node;\
	char *sanchor;\
\
	sanchor = st->current;\
	func(st);\
	if (st->status == MATCH && st->node != NULL) {\
		node = node_new();\
		node_fill(node, sanchor, st->current - sanchor, #func, strlen(#func));\
		node->right = st->node;\
		st->node = node;\
		st->left = st->node;\
	}\
} while(0);\

#define REPEAT(n, m, body) \
do {\
	Node *node;\
	Node *nanchor;\
	char *sanchor;\
	int i;\
\
	sanchor = st->current;\
	nanchor = NULL;\
	for (i = 0; i < m || m == INF; i++) {\
		body\
		if (st->status == FAIL) {\
			break;\
		}\
		if (st->status == MATCH && st->node != NULL) {\
			if (i == 0) {\
				nanchor = st->node;\
				node = st->left;\
			} else {\
				node->left = st->node;\
				node = st->left;\
			}\
		}\
	}\
	if (i < n) {\
		st->status = FAIL;\
		st->current = sanchor;\
		node_free(nanchor);\
		node_free(st->node);\
	} else {\
		st->left = node;\
		st->node = nanchor;\
		st->status = MATCH;\
	}\
} while(0);\

void	 node_fill(Node *, char *, int, char *, int);
void	 node_free(Node *);
Node	*node_new(void);
void	 printtree(Node *, int);

/* see RFC 5234 - Appendix B.1. */
void	 alpha(State *);
void	 bit(State *);
void	 crlf(State *);
void	 digit(State *);
void	 dquote(State *);
void	 hexdig(State *);
void	 htab(State *);
void	 lpar(State *);
void	 lwsp(State *);
void	 octet(State *);
void	 rpar(State *);
void	 sp(State *);
void	 vchar(State *);
void	 wsp(State *);

extern Node *tree;

#endif
