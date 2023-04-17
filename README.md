# Usage
This library allows easy parsing of ABNF written grammar, by providing a "compilable metalanguage" as a set of C macros. The library automatically build a C **binary** tree of the grammar in case of syntax matching.
The use of the library consists in writing the grammar ABNF rules as C functions (one function per rule). Rule functions must have the following C prototype (literally the same, except for the function name - i.e. the `State *` parameter must be named `st` - it's important for the macros to work properly) :
> `void rulename(State *st);`

The function body shouldn't contains anything other than blocks (see *Blocks* section), that describe the rule content. Blocks must be used as it inside of rule functions (i.e. no need of init process or local variables declaration).

The use of the parser goes through a `State` struct which must be defined and initialized by the calling function (e.g. `main()`). This struct makes it possible to transmit the text to be parsed, as well as the subsequent retrieval of the syntax tree.

**NB: The text must be syntactically correct (i.e. `st.status == MATCH`) to return a valid syntax tree. If the text is syntactically incorrect `st.status == FAIL`, and the syntaxe tree is irrelevant.**

The definition of State is as follows:
```
typedef struct {
	Node *node;     /* syntaxe tree */
	Node *left;
	char *current;  /* next character to be parsed */
	int status;     /* syntaxe matching status (FAIL or MATCH )*/
} State;
```
A typical `State` initialization would be:
```
int
main(int argc, char *argv[])
{
	char *s = "foo";
	State st = { NULL, NULL, s, MATCH };
	return 0;
}
```
The next step is to pass the `State` struct to the "start rule" (i.e. the one which isn't in any ABNF rule right part) :
```
static void	 start(State *);

int
main(int argc, char *argv[])
{
	char *s = "foo";
	State st = { NULL, NULL, s, MATCH };

	start(&st);
	return 0;
}

static void
start(State *st)
{
	STR(foo)
}
```
The library provide the `printtree(Node *tree, int indent)` function to print the returned syntax tree (where `tree` is the syntax tree, and `indent` the base indent in number of htab) :
```
static void	 start(State *);

int
main(int argc, char *argv[])
{
	char *s = "foo";
	State st = { NULL, NULL, s, MATCH };

	start(&st);
	if (st.status == MATCH)
		printtree(st.node, 0);
	return 0;
}

static void
start(State *st)
{
	STR(foo)
}
```
```
% ./main
string: "foo"
```
## NB
The following ABNF basic rules are provided by the library as rule function (see *RFC 5234 - Appendix B.1*.):
```
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
```
# Blocks
## `ASCIIRANGE(n, m)`
### ABNF
- `rulename = %xn-m`
- `rulename = %xn`
### C
 ```
void
rulename(State *st)
{
	/* rulename = %xn-m */
	ASCIIRANGE(n, m)
	/* rulename = %xn */
	ACIIRANGE(n, n)
}
```
## `STR(s)`
`STR()` is case insensitive, use `ASCIIRANGE()` for case sensitive.
### ABNF
- `rulename = "s"`
### C
```
void
rulename(State *st)
{
	STR(s)
}
```
## `CHOOSE(block)` & `OR(block)`
### ABNF
- `rulename = block1 | block2 | ... | blockn`
### C
```
void
rulename(State *st)
{
	CHOOSE(
		OR(block1)
		OR(block2)
		...
		OR(blockn)
	)
}
```
## `EVERY(block)` & `AND(block)`
### ABNF
- `rulename = block1 block2 ... blockn`
### C
```
void
rulename(State *st)
{
	EVERY(
		AND(block1)
		AND(block2)
		...
		AND(blockn)
	)
}
```

## `REPEAT(n, m, block)`
`m` and `n` must be an integer greater or equal to 0, or the `INF` constant.
### ABNF
- `rulename = n*m(block)`
- `rulename = n*(block)`
- `rulename = *m(block)`
### C
```
void
rulename(State *st)
{
	/* rulename = n*m(block) */
	REPEAT(n, m,
		bloc
	)
	/* rulename = n*(block) */
	REPEAT(n, INF,
		bloc
	)
	/* rulename = *m(block) */
	REPEAT(0, m,
		bloc
	)
}
```
## `OPTIONAL(block)`
### ABNF
- `rulename = [ block ]`
### C
```
void
rulename(State *st)
{
	OPTIONAL(block)
}
```
## `RULENAME(rulename1)`
### ABNF
- `rulename = rulename1`
### C
```
void
rulename(State *st)
{
	RULENAME(bit)
}

void
bit(State *st)
{
	/* bit = %x30-31 ;[0-1] */
	ASCIIRANGE(0x30, 0x31))
}
```
# Examples
### ABNF
```
date            =       day month year
year            =       4*DIGIT
month           =       FWS month-name FWS
month-name      =       "Jan" / "Feb" / "Mar" / "Apr" /
                       "May" / "Jun" / "Jul" / "Aug" /
                       "Sep" / "Oct" / "Nov" / "Dec"
day             =       1*2DIGIT
```
### C
```
void
date(State *st)
{
	EVERY(
		AND(RULENAME(day))
		AND(RULENAME(month))
		AND(RULENAME(year))
	)
}

void
year(State *st)
{
	REPEAT(4, INF,
		RULENAME(digit)
	)
}

void
month(State *st)
{
	EVERY(
		AND(ASCIIRANGE(0x2f, 0x2f))
		AND(RULENAME(month_name))
		AND(ASCIIRANGE(0x2f, 0x2f))
	)
}

void
month_name(State *st)
{
	CHOOSE(
		OR(STR(Jan))
		OR(STR(Feb))
		/* ... */
		OR(STR(Dec))
	)
}

void
day(State *st)
{
	REPEAT(1, 2,
		RULENAME(digit)
	)
}
```