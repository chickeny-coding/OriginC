# OriginC

## Features

OriginC is a compiler which can compile `.og` file into `.s`.

The generated `.s` file can be compile into executable with **Linux** `gcc` directly.

If the source code is invalid, OriginC will print the error message to `stderr` and exit the compilation with non-`0` code.

## Usages

* Clone the repository from `GitHub` with `git`.

> Make sure you have `git`

```sh
git clone git@github.com:chickeny-coding/OriginC.git
```

* Go into the repository.

```sh
cd OriginC/
```

* Build the compiler.

> Make sure you have `make`

```sh
make
```

* Now you can compile your `.og` file.

```sh
# src.og: source file
# dst.s: destination file
./ogc src.og dst.s
```

* Then compile it into executable with **Linux** `gcc`.

> Make sure you have **Linux** `gcc`.
>
> OriginC requires standard `C` version `C23`.
>
> Only standard `C23` and `gcc` are enough for building.

```sh
gcc dst.s -o dst
```

* At last, run it.

```sh
./dst
```

## Syntax

### Global

A global is a whole program.

It must be a block with a pair of parentheses around.

### Block

A block is several items in a pair of parentheses.

It can be nothing, an identity and a block, or a block with a pair of parentheses around and another block.

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
