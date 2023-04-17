# Blocs
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
## `CHOOSE(bloc)` & `OR(bloc)`
### ABNF
- `rulename = bloc1 | bloc2 | ... | blocn`
### C
```
void
rulename(State *st)
{
	CHOOSE(
		OR(bloc1)
		OR(bloc2)
		...
		OR(blocn)
	)
}
```
## `EVERY(bloc)` & `AND(bloc)`
### ABNF
- `rulename = bloc1 bloc2 ... blocn`
### C
```
void
rulename(State *st)
{
	EVERY(
		AND(bloc1)
		AND(bloc2)
		...
		AND(blocn)
	)
}
```

## `REPEAT(n, m, bloc)`
`m` and `n` must be an integer greater or equal to 0, or the `INF` constant.
### ABNF
- `rulename = n*m(bloc)`
- `rulename = n*(bloc)`
- `rulename = *m(bloc)`
### C
```
void
rulename(State *st)
{
	/* rulename = n*m(bloc) */
	REPEAT(n, m,
		bloc
	)
	/* rulename = n*(bloc) */
	REPEAT(n, INF,
		bloc
	)
	/* rulename = *m(bloc) */
	REPEAT(0, m,
		bloc
	)
}
```
## `OPTIONAL(bloc)`
### ABNF
- `rulename = [ bloc ]`
### C
```
void
rulename(State *st)
{
	OPTIONAL(bloc)
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