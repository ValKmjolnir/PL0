# PL/0 @ NUAA CCST 2020 Compiler Principles

Content

- [PL/0 compiler and virtual machine](#pl0-compiler-and-virtual-machine)
  - [What is `LL(1)`?](#what-is-ll1)
  - [What is PL/0?](#what-is-pl0)
- [Build](#build)
- [Usage](#usage)

## PL/0 compiler and virtual machine

This is a course design for `Compiler Principles` of `NUAA CCST 2020`.
It includes a PL/0 bytecode compiler and virtual machine.

This repo was used to store the code, but now let's make it more useful. This repo will help you to learn a simple implementation of compiler of a simple language PL/0, which uses `LL(1)` grammar.

### What is `LL(1)`?

`LL(1)` grammar means that every production rule can be determined by the first symbol of itself. To make it more understandable, we can use a simple example to illustrate it.

Look at the following grammar:

```ebnf
<code_block> ::= "begin" {<stmt>} "end"
<stmt> ::= <condition> | <loop>

<loop> ::= "while" <expr> "do" <code_block>
<condition> ::= "if" <expr> "then" <code_block> ["else" <code_block>]
```

As we could see, `<stmt>` accept two kinds of statement: `<condition>` and `<loop>`. But how to distinguish them?

And abviously, `<condition>` alawys starts with `"if"`,
and `<loop>` always starts with `"while"`. So the answer is:

```C++
if (lookahead(tok_kind::keyword_if)) {
    // parse <condition>
} else if (lookahead(tok_kind::keyword_while)) {
    // parse <loop>
}
```

And we could see that we could use `lookahead()` to check the next `1` token, then we will definitely know what kind of statement we will parse. That's what `1` means in `LL(1)`.

For `LL(k)`, `k` means some of the rules are determined by the next `k`(by maximum) tokens.
In fact most of the languages used in the industry are `LL(1)` with some special cases.
And of course, some of these languages are `LL(k)`.
Languages using `LL` grammar are usually easier to maintain.

But `LL(1)` only requires one token lookahead.
So it is easy to implement the parser.

### What is PL/0?

PL/0 is a simple language. It is a subset of Pascal.
The grammar is really simple, so it only requires `LL(1)` parser.
Here's the grammar:

```ebnf
<program> ::= "program" <id> ";" <block>

<decl> ::= <var-decl>
         | <const-decl>
         | <procedure-decl>
<var-decl> ::= "var" <id> [":=" <expr>] ";"
<const-decl> ::= "const" <id> [":=" <expr>] {"," <id> [":=" <expr>]} ";"
<procedure-decl> ::= "procedure" <id> "(" [<param-list>] ")" ";" <block>
<param-list> ::= <id> {"," <id>}

<block> = {<decl>} <body>
<body> = "begin" <stmt> {";" <stmt>} "end"
<stmt> ::= <assign-stmt>
         | <if-stmt>
         | <while-stmt>
         | <call-stmt>
         | <read-stmt>
         | <write-stmt>
         | <block>
<assign-stmt> ::= <id> ":=" <expr>
<if-stmt> ::= "if" <expr> "then" <stmt> [ "else" <stmt> ]
<while-stmt> ::= "while" <expr> "do" <stmt>
<call-stmt> ::= "call" <id> "(" [<actual-param-list>] ")"
<actual-param-list> ::= <expr> {"," <expr>}
<read-stmt> ::= "read" "(" <id> {"," <id>} ")"
<write-stmt> ::= "write" "(" [<expr> {"," <expr>}] ")"

<expr> ::= <term> {("+" | "-") <term>}
<term> ::= <factor> {("*" | "/") <factor>}
<factor> ::= <id>
           | <number>
           | "(" <expr> ")"
           | "-" <factor>
```

## Build

use this command to compile this project:

```bash
make
```

## Usage

To get help:

```bash
./pl0 -h
```

use this command to use pl0 interpreter:

```bash
./pl0 -exec <filename>
```

use this command to use pl0 interpreter(debug):

```bash
./pl0 -debug <filename>
```
