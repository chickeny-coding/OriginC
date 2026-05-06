# OriginC

## Features

OriginC is a compiler which can compile `.og` file into executable.

If the source code is invalid, OriginC will print the error message to `stderr` and exit the compilation with code `1`.

## Usages

* Clone the repository from `GitHub` with `git`.

> Make sure you have `git`.

```sh
git clone git@github.com:chickeny-coding/OriginC.git
```

* Go into the repository.

```sh
cd OriginC/
```

* Write your own `.og` file.

> You had better to name it `src.og` because then you can just compile it with `Makefile`.

* Build the compiler.

> Make sure you have `make`.
>
> `Makefile` does 3 things: build OriginC, compile `src.og` into `dst.s` with `ogc`, and compile `dst.s` into `dst` with **Linux** `gcc`.
>
> You can remove all files built by `Makefile` with `make clean`.

```sh
make
```

* At last, run it.

```sh
./dst
```

## Syntax

### Global

A global is a whole program.

It must be several functions.

The function `main` will be the entry of the program.

Missing `main` function can occur linking error.

### Function

A function is a named block, which can call other functions or be called by other functions.

The definition of a function is the function name and the function block.

It must be a block with a pair of parentheses around.

> Calling recursively is allowed.
>
> But endless recursion can occur Segmentation Fault.

### Block

A block is several items in a pair of parentheses.

An item can be:

* Another block with a pair of parentheses around.
* A function call, which is a `!` and the function name.
* An identity, which will print the identity.

### Identity

An identity can be understood as a special string.

It is a string that doesn't include:

* `Whitespaces`, which means its `ASCII` code is not greater than `32`.
* `BlockLeft`, which is `(`.
* `BlockRight`, which is `)`.

All identities are printed in **pre-order** of the program AST separated with `\n`.

## Examples

### 0 Single Line and Multiple Lines

#### Code 0

```og
main
(
  print-in-single-line
  print in multiple lines
)
```

#### Output 0

```txt
print-in-single-line
print
in
multiple
lines
```

### 1 Nested Block

#### Code 1

```og
main
(
  (
    (
      (
        (
          (
            (
              (
                nested-here
              )
            )
          )
        )
      )
    )
  )
)
```

#### Output 1

```txt
nested-here
```

### 2 Calling Other Functions

#### Code 2

```og
f
(
  f
)
main
(
  before-calling
  !f
  after-calling
)
```

#### Output 2

```txt
before-calling
f
after-calling
```

### 3 Calling Itself Recursively

#### Code 3

```og
main
(
  main
  !main
)
```

#### Output 3

```txt
main
main
main
...
main
Segmentation fault (core dumped)
```
