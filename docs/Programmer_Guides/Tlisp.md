# TLisp

Transcendence uses a [Lisp](https://en.wikipedia.org/wiki/Lisp_(programming_language))
variant known as TLisp for scripting. In Lisp both code and data are written using
symbolic expressions (sexps) in the form of lists of data, code (and other lists). As
a result Lisp code will contain a lot of parentheses, so it is highly recommended you
use an editor which highlights matching brackets.

TLisp scripts are formed of two basic components: atoms and lists. The atoms represent
numbers, strings, and functions; while lists are sequences of atoms and/or other lists
separated by spaces. For example, `(1 "two" 3)` is a list comprising three atoms: `1`,
`"two"`, and `3`.

Expressions are written as lists using prefix notation - where the first element in the
list is the name of a function. So to add three numbers together you would write:
```lisp
(+ 1 2 3)
```
which returns `6`

## Expressions and quoting

TLisp will evaluate every expression you give it, this means that typing `(+ 1 2 3)`
as above will immediately evaluate to `6`. Basic atoms (strings and numbers) will
evaluate to themselves e.g. `3.0`, `"string"`. However, the expression `(1 2 3)` is
not valid as TLisp will attempt to call a function named `1`. In order to create a
list you should use the `list` function for example:
```lisp
(list 1 2 3)
```
Alternatively you can use the special function `quote` which returns its argument
without evaluating it. You can also a single quotation mark as an abbreviation `'`:
```lisp
; The following both return a three element list (1 2 3):
(list 1 2 3)
(quote (1 2 3))
'(1 2 3)

; But note the difference between the following:
(+ 1 2 (+ 2 3))         ; 8
(list + 1 2 (+ 2 3))    ; ([function: +] 1 2 5)
'(+ 2 3 (+ 3 3))        ; (+ 1 2 (+ 2 3))
```

## Nil

In TLisp `Nil` is a special atom equivalent to an empty list `()`. These represent
the "false" state in any logical tests, with any other atom or non-empty list being
"true". This means all integers (including 0) and strings (including empty string)
are equivalent to "true".

`Nil` and empty lists `()` and structs `{}` are interchangeable and can be considered
identical except one specific case - if you perform an in-place list modification
operation on a `Nil` value it will return a new actual list, while in-place modification
of an empty list does not need to generate a new list. Similarly sets can also be
modified in-place like lists.

**NOTE** if you need to generate an empty list then use the `(list)` function as the
quote forms will return a Nil value

```lisp
; Evaluating an empty list is equivalent to function call.
()                      ; Returns Nil
Nil                     ; Returns Nil
(list)                  ; Returns (), equal to Nil
{}                      ; Returns {}, equal to Nil

; quote forms return Nil
'()                     ; Returns Nil
(quote ())              ; Returns Nil
```

## Boolean values and Conditionals

Although Tlisp has a specific atom (non-mutable) for `True` it does not have the same
concept of `True` and `False` as in other programming languages. Instead this is
done with the concepts of `Nil` and `Non-Nil`.

* If something is `Nil` it is treated as false
* If something is not `Nil` it is treated as true

Tlisp only treats specific values as `Nil` when evaluating conditional or boolean
statements like `(if ...)` or `(and ...)`

```lisp
; The following values are treated as Nil and therefor false
Nil
(list)
{}

; They are also all treated as equal
(= Nil (list) {}) -> True
```

Everything other than these two values is treated as being non-Nil:

```lisp
; The following are some examples of values that are not Nil and therefor are treated as True
""
0
1
(double 'Nan)
{ a:1 }

; Unlike Nil however they are not considered equal to True (the atom)
(= 1 True) -> Nil
```

As a result the conditional statements are not actually checking if the conditional expression
returns `True` but instead if it is `Non-Nil`

```
; Empty strings are considered Non-Nil
(if "" "was true" "was false") -> "was true"

; or returns the first Non-Nil value
(or Nil {} 0) -> 0

; and returns the first Nil value
(and 0 1 2 3 4 {} 5 Nil) -> {}
```

## Data types

* Atoms
  * Nil
  * True
  * Numeral
    * Integer
    * Real
  * String
  * Function
    * Primitive
    * Lambda
* Lists
* Structs
* Vectors
