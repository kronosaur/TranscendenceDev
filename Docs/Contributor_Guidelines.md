# TRANSCENDENCE CONTRIBUTOR GUIDELINES

This article contains coding conventions for the Transcendence source code:
https://github.com/kronosaur/TranscendenceDev

## Introduction

Transcendence started as a project literally last century. Since then, it has
grown to over a half million lines of code (about half C++ code and half XML/TLisp).
The source code is like an ancient city, built over generations. Glass and
steel skyscrapers stand next to old wooden buildings.

This article describes the coding conventions for new code. This is how we want
the source code to eventually look. But we realize that we cannot always
rewrite existing code. Think of it as building regulations: we want new
buildings to adhere to them, but older ones get a pass (for now).

This article is a work in progress. Over time we will add more guidelines; we
might even change previous guidelines. If you have a particular coding
convention question or if you wish to suggest a guideline change or addition,
please add a ticket on [ministy](https://ministry.kronosaur.com) or stop by
the [discord](https://forums.kronosaur.com/viewforum.php?f=61) or
[forums](https://forums.kronosaur.com).

## General Guidelines

The following guidance is intended to be relevant regardless if you are adding
new features to the C++ engine code, refactoring it to support features from
the latest C++ versions, or working with the XML and tlisp gamedata files.

### The Team Is the Judge

Code quality is a relative metric. Different teams have different preferences,
experiences, and strengths. Code that is perfectly obvious to one team might
be confusing or error-prone to another.

When judging whether a piece of code is good or not, think of our specific team.
Is this a common pattern in the source code? Is this how the team wants to
evolve the code? Will this confuse the team?

Historically George Moromisato has been the primary contributor to this repository.
Not only did he start the game, but he also contributed the majority of the codebase
(over 90% by lines, including 3rd party libraries, as of 2025).
However we want to build this codebase into one that attracts more developers,
and to improve this engine, its APIs, and its playable adventures to attract more
players and modders.

### Pull Requests

Keep in mind when making pull requests that the team has limited time each week
to review changes.

*  Pull requests that are too large may be rejected as too large to reasonably
    review - if that is the case, please break up your change into several pull
    requests.

* Is is best to keep a PR to a single feature, fix, or refactoring.

* If your PR is not ready to merge, mark it as a draft PR.

* It is currently recommended but not mandatory to have an associated ticket on
    [Ministry](https://ministry.kronosaur.com) with your pull request.

### "When in Rome, Act like a Roman."

When writing new code or evolving existing code, think about the surrounding
context and make sure your code fits in.

* Within a function, keep the conventions of the function. If it's necessary
    or desirable to change the conventions, make sure the entire function is
    changed. Don't change a small piece of it.

* Within a class, try to keep the conventions of the class. If possible,
    change the whole class to evolve to a new convention. In some cases,
    however, a class is so big that it is impossible to evolve the whole class,
    but we don't want to keep adding old-style code. For example, CSpaceObject
    is a large class. It is OK to evolve small pieces of it as long as the
    pieces can be logically grouped.

### Performance Is Important

Performance, particularly graphics performance, is important. Even at 60 frames
per second we only have 16 milliseconds to paint a frame. That means processing
physics, AI, and effects for hundreds of objects, and painting millions of pixels
as fast as possible.

* Worry about algorithmic performance. Don't do an n-squared loop if you can
    sort things and do n-log-n.

* Worry about inner loops. If you're iterating over every ship every frame,
    then that code better be very fast. If possible, cache information rather
    than recomputing. Even better, don't do work you don't need to do.

* Don't worry too much about micro optimizations. The overhead of a function
    call once per frame (or even a thousand times per frame) is not material.
    Don't try to inline just to save that cost.

* On the other hand, if you're looping over every pixel on the screen, even
    function call overhead can make a difference.

* When in doubt, measure.

### Safety Is Important

Code safety is about preventing bugs:

* Design you functions and classes to be resistant to bugs. Think about how a
    caller might misuse the function and try to design against it. For example,
    if an object argument is required, make it a reference instead of a pointer
    argument.

* Avoid artificial restrictions on the caller. For example, a class in which
    some methods cannot be called unless an initializer method is called first
    (initialize in constructor instead).

* Prefer smart pointers and RAII patterns.
