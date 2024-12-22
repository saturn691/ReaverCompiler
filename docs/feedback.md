# Reaver Feedback:
## Overall
You passed 181 / 201, giving you an overall score of 90.0%, with a seen to unseen split as follows:
- 86 / 86 seen (100%)
- 95 / 115 unseen (82.6%)

Overall a superb effort, amazing job on getting all seen test cases passing, and so many of the unseen ones. You clearly both put a lot of effort into this project, and have achieved an outstanding result. It was great to see you also implement such an extensive suite of (74!) custom tests, I'm sure this helped you catch many bugs in your implementation, and is probably what enabled you to be the only team to crack the 90% boundary, congrats (although some others were _very_ close behind)!

Considering the complexity of the functionality you implemented (I think you might be the first team ever to support multi-dimensional arrays?), you dropped quite a few "easy" marks (more on that below), but your general approach to structuring your compiler was quite excellent, avoiding lots of common "traps" students fall in to. That said, there are still quite a few things that I think could be improved - I'm hoping you'll find the feedback below useful. Excellent work guys 💪.

## General / DevOps Feedback
- Decent file structure, I really like how you split up your headers and grouped them in logical units with folders. However, most of your code is implemented directly in your header files, which isn't best practice as it can lead to ODR violations (which isn't relevant in your case since you're only defining class methods which are marked [`inline`](https://en.cppreference.com/w/cpp/language/inline) by default) but can dramatically increase build times when recompiling.
  - You also define these methods directly in your class rather than separately, which makes the API of these classes harder to read. Read more [here](https://stackoverflow.com/a/63705851/21995872).

- You have a lot of compiler warnings when building your project (although most of them are hidden by `-Wno-unused-parameter` in your Makefile). You already removed some of these flags disabling warning from the makefile, so it would've been good for you to remove this one as well and address the warnings - they give valuable information on stupid mistakes. Ideally you should add `-Werror` to force you to fix these.
  - If you have a lot of unused parameters, this can be an indication of code smell - ask yourself if you really need all these parameters, or if you can't wrap them up in a nicer way. That said, if you have need to have these parameters, a nice way of dealing with them can be either not naming them when declaring / defining the function you're overriding, or labelling them as [`[[maybe_unused]]`](https://en.cppreference.com/w/cpp/language/attributes/maybe_unused)
  - I'm surprised you left 14 shift/reduce conflicts in your parser, these usually indicate that something is actually wrong in your parser. Fortunately in your case, it only seems to have directly cost you 1 unseen test, where you were unable to parse `typedef double arr[32]`, but it would've been smart to try and fix these.

- Excellent use of pull requests (and code reviews!!!) to implement functionality (although I had to go to [your actual repo](https://github.com/saturn691/ReaverCompiler) to see these as they weren't carried over to your submission, not your fault though). Also very impressive to see you get started on this so early, guess the lectures aren't needed 😂.

- Fantastic job writing such an extensive test suite!

- Most of the tests you fail are the result of an intentional crash in your compiler - excellent work! In general it's much better to know your software is wrong from a hard crash then when it silently produces incorrect output (although for the purposes of maximising number of passing test cases it's not optimal as you don't give yourself any room to get lucky).

## Code Feedback

- Beautiful `Node` interface, nice work keeping it so simple!
  - You should prefer to store your child `Node`s as `const Node*` rather than `Node*` - once you've built your tree it should be immutable, with all necessary state-tracking happening in your context class.
    - Even better, use a `std::unique_ptr<const Node>` to have all the memory management taken care of for free!
  - You use `dynamic_cast` a bit more than I'd like - try to avoid using it, as it's usually an indication of code smell. That said, you could've definitely used it a lot more than you did, so nice work in designing your AST in such a way where you only needed to use it in a few specific cases.
    - For example, in `Assign`, you could avoid using `dynamic_cast` by storing a `std::variant<ArrayAccess*, UnaryExpression*, Expression*>`, which would allow you to make it very clear that you expect a certain set of expresssions, which each have their own distinct behaviour. That said, I think this is a case where you could generalise better to make your code cleaner, as all you want to do in an assignment is evaluate the `assignment_expression` and store it in the address of the `unary_expression`. By properly encapsulating this functionality into your respective AST classes, I think it would be feasible to completely remove this class-dependent behaviour in `Assign` and simplify your code, which should reduce potential bugs.
  - You should consider the use of the [`final`](https://en.cppreference.com/w/cpp/language/final) specifier for terminal AST classes (/ methods in intermediate classes). This helps document your code by indicating what your concrete classes are, as well as help the compiler warn you if you ever try to accidentally override the method of one of these terminal types.
  - You sometimes incorrectly choose to have classes inherit from `Node` when it doesn't make sense for them to. For example, `AssignOp` should not be a `Node` - it can't `gen_asm`! Just using `AssignOpType` directly would be much better, and letting this be information be owned by `Assign`.

- You leak a bunch of memory, most notably because you never `delete` your tree you get from `parseAST()` in your main `compile` function (although you leak it in a bunch of other places too). Obviously leaking memory in this way isn't a huge deal, since the memory will get cleaned up by the OS once your program terminates, but is bad practice and something you should make a habit of trying to avoid. Using a [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr) could help avoid this, in modern C++ code it's incredibly rare to see explicit calls to `new` and `delete`, as using smart pointers and RAII makes it quite easy to manage resources properly with zero-overhead.

- You have a fair amount of comments in your code, some of which are good, and some which aren't particularly helpful (bad). Comments should always be about *why* your code is a certain way, and not *what* the code is doing (I can see what the code is doing since I can read it).
  - For example, in the following snippet the comments aren't really adding anything:
    ```c++
    // Initialise it with the value
    // If it's a PointerDeclarator, then it's an INT type
    Types type;
    if (dynamic_cast<PointerDeclarator*>(declarator))
    {
        type = Types::INT;
    }
    else
    {
        type = context.current_declaration_type->get_type();
    }
    ```
  - Your block `/**/` comments tended to be quite informative.

- You should prefer using `std::optional` when you want semantics of "something or nothing". Something as follows is much more difficult to reason about and more error prone:
  ```c++
  // bad
  std::string input_reg = "don't care";

  // good
  std::string input_reg = std::nullopt;
  ```

- Nice use of `enum class`es for things like `Types`, `Scope`, `OperatorType`, etc.
  - You should avoid using the `default` label in `switch` statements, particularly when the `switch` is over an `enum class`, but rather deal with each `case` explicitly. This is because the compiler won't warn you you're missing a case when you have the `default` label, making it much difficult to figure out if you've accidentally missed something, or see where you need to make changes when you add a new value to your `enum class`.
    - You should also avoid defining mappings for `enum class`es using a `std::unordered_map`, for example in `Operator::symbol_map` for the same reason.

- Your `Context::add_enum_value()` is wrong in a number of ways.
  - Using `-1` to represent a value not being set is incorrect, as it is perfectly valid code to assign an enum the value `-1`, as well as values larger than an `int`, meaning your compiler will generate incorrect values in this case. As mentionned previously, you should really use `std::optional` when you want to have 'optional' semantics, it's much safer and clearer than magic invalid values.
  ```c
  enum Foo
  {
    BAR = -1, // Will assign the value 0 instead of -1
    BAZ = 0xFFFFFFFFF // Will truncate
  };
  ```
  - Your `enum_next_value` doesn't update when `val` is provided (which causes you to fail 1 testcase). This could also result in you assigning duplicate enum values if someone explicitly numbers one of their `enum` values and not others.
  ```c
  enum Foo
  {
    BAR = 4,
    BAZ // = 5 (your compiler assigns 0)
  };
  ```

- It looks like you based yourself on the ANSI-C grammar for your parser, nice work spending the time going through and (mostly) understanding the grammar (although as mentioned before it would've been good to clean up errors you left there). You could've also considered deleting unused parts of the grammar rather than just commenting they weren't necessary (although still nice job on commenting).
  - You could've reused existing logic to avoid some complexity in your classes. For example, instead of storing the `AssignOp` in your `Assign` (and then later constructing a temporary `Operator` to handle the specific logic of each type), you could have done this directly in your parser, resulting in a much simpler `Assign`:
  ```c++
  assignment_expression
      : conditional_expression { $$ = $1; }
      | unary_expression '=' assignment_expression { $$ = new Assign($1, $3); } // No need for AssignOp anymore
      | unary_expression MUL_ASSIGN assignment_expression { $$ = new Assign($1, new Operator($1, $3, OperatorType::MUL)); }
      // etc...
      ;
  ```
  - You can use this same technique to implement pre-increment / decrement operations (or even post-increment / decrement but this is more complex due to needing to return the old value). This could've helped keep your `PostIncrement` class simpler (which could've possibly saved you marks - see below). It's also very misleading to make your `PostIncrement` class actually pre-increment based on the value of a boolean flag, this is quite stinky.
  ```c++
  unary_expression
      : postfix_expression { $$ = $1; }
      | INC_OP unary_expression { $$ = new Assign($2, new Operator($2, new Number("1"), OperatorType::ADD)); } // Number storing a string is some slight overcookery imo 😅
      // etc...
  ```
    - As a general piece of feedback, it's good to try and keep your classes as small as possible. You can always extract common functionality into parent classes, but having complicated conditional logic to handle different scenarios in a class makes it much more brittle when it starts interacting with other classes. Really try to focus on keeping the code as simple as possible; as I'm sure you've noticed things start getting really complicated as they grow.

- Nice work figuring out how to keep track of things with your `Context` class, there were a lot of non-trivial problems here to think about and you seem to have figured out nice ways to solve them. That said, I also think it's the part of your code that could use the most improvements.
  - Nice use of appropriate STL classes.
    - `[nit]` Using a `std::stack` for your scopes is nice, but requires a lot of unnecessary copying. I think it's actually cleaner in this case to use a `std::vector`, and when looking for variables you start at the end of your vector and look backwards, meaning you never need to copy all of the existing variables in your stack into the new stack frame.
  - `[nit]` Prefer `using Name = type` to `typedef type Name`, it's considered more modern.
  - I'm not sure how I feel about your `Context` being responsible for assembly generation, it feels a bit like a mixing of concerns (i.e. `Context` is in change of managing state, AST is in charge of generating assembly).
  - There are a couple places where you've had to overfit your `Context` for niche use cases, as you claim there's "no way for (a child) to get the information down from it's parent", but this is not true - you control the API of your classes. By having the parent store more specific pointers to the children, you could simply update the API of the children to make passing down this additional context possible, without needing to clutter your global context. You don't necessarily want your `Context` to be an intersection of what your Nodes need, but being more critical on what gets included allows you to avoid having to put literally everything in there.
  - I can't believe you guys were too lazy to calculate how much stack space was needed for your functions. Like you did multi-dimensional arrays lmao... We still caught you with 1 testcase though :P (`int arr[500]` was too much for you to handle). Nice job crashing hard on it at least.
    - Side note: It's actually very difficult for us to verify that students allocate stack memory properly, since you either need to use arrays (advanced feature), or have a testcase with 1000s of local variables (which we don't want to write). Not sure if you guys considered this when choosing to use this hack, but if so very cunning.
  - `Mode` feels like a bit of a hack, it adds a lot of state that you need to think about when implementing your AST `Node`s, and I'm not convinced that it's necessary, I think that by storing the appropriate types with appropriate interfaces in your AST in can be context aware enough to generate the assembly it needs to.
    - I'm also not sure why some of these modes are needed, you shouldn't really be generating assembly differently when you're in a `return` statement to any other case (ignoring optimisations). `Mode::RETURN` is actually a source of error for you - see below.
  - When dealing with constants in a class (e.g. `registers_default`, `register_map`, etc...), I'd recommend marking them as `static`, meaning they are shared among all class instances. There's no reason having a copy of each of these in every instance of your class (not really relevant since `Context` is a singleton), but more importantly it documents that these constants don't rely on any internals of your class, but their own self contained things (which are logically linked to this class in some way).
    - `[nit]` You should also prefer [`constexpr`](https://en.cppreference.com/w/cpp/language/constexpr) over `const` whenever possible, this ensures that the values are evaluated at compile-time rather than relying on compiler optimisations to avoid evaluating them at run-time. These are also better than macros - avoid these as much as possible in C++ (the only use-case where they're valid is when you want to avoid evaluating function arguments if not necessary, like when implementing logging functionality).

- Nice work sticking to a consistent naming scheme (although camelCase > snake_case, we're not writing C code here)
  - It's also worth incorporating an easy way to differentiate between member variables and "regular" variables. Common patters are `mMemberVariable` or `member_variable_`.

- `[nitpick]` Using `#pragma once` instead of `#ifndef ... #define ... #endif` is better for header guards as it is less prone to manual error.

### Bugs
Given the tremendous effort you put in as a team, I've done my best to identify the cause of all failing test cases in your compiler (which include a couple above) - from what I can tell only 2 were due to "known limitations" in your compiler, i.e. where you actively decided not to implement features, and the rest were from bugs (quite a few of which are quite unlucky).

- You have an unfortunate "bug" where you can't assign to global variables properly, causing even simple test cases like the following to fail:
    ```c
    int x = 10;

    int ffff()
    {
        int y = 1;
        x = x + y;
        return x;
    }
    ```
    The issue is that the assembly you generate is invalid, as you're missing the `%lo()` needed to write to this variable. For example, in the case above you generate the assembly:
    ```
    // ...
    // Actual assignment in Context::store
    sw t1, x
    // ...
    ```
    vs godbolt:
    ```
    sw      a4,%lo(x)(a5)
    ```
    - Whilst I think you handled this properly when assigning to global arrays, you have a separate issue where you don't define a global array, i.e. you're missing the following assembly:
        ```
        arr:
                .zero   40
        ```
        Missing this assembly results in a linker error if this global array is not declared in any other source file (i.e. your driver code), as it ends up not being defined anywhere. This only materialises in (assessed) test cases where you assign to global arrays, as in other cases the array is either initialised by the driver, or initialised in the array declaration itself which you seem to have managed correctly.
  - Overall these issues only cost you 4 cases, but it's unfortunate that you didn't think to test this behaviour as you could've caught it quite easily (at least for global variables, the array one is definitely harder to catch).

- You have another bug where you don't correctly handle string literals defined as arrays rather than pointers (tbh how you handle `char[]` might just be wrong?). I suspect this has to do with how you store the address, as you appear to only store bytes rather than the whole word, but I'll leave it to you to fully debug it (if you want) as it only affects 1 test case
    ```c
    int g()
    {
        char str[10] = "hello";
        return str[4];
    }
    ```
    The relevant snippet of what you produce is:
    ```
    g:
        addi sp, sp, -512
        sw ra, 508(sp)
        sw s0, 504(sp)
        addi s0, sp, 512
        addi a0, s0, -27
        lui t0, %hi(.string0)
        addi t0, t0, %lo(.string0)
        # You seem to be using t0 to store the start of the string, so not sure why you're only storing a byte here
        sb t0, -27(s0)
        li t0, 4
        li t1, 1
        mul t0, t0, t1
        # This is also sus, you now want to add the pointer of the string to t0 to get the correct value, but instead you seem to be adding the address of where you stored pointer to the string?
        addi t0, t0, -27
        add t0, t0, s0
        lb a0, 0(t0)
        j g_end
        # Only gets here if function body empty
        li a0, 0
    ```
- You had a couple issues with how you managed structs:
  - You don't handle nested structs correctly due to how your context works (although it seems you were aware of this) [1 testcase]
  - You produce incorrect assembly when doing `sizeof(struct x)`. What's very sad here is that you had coverage in your own tests of `sizeof(b)` where `b` is an instance of `struct x`, but didn't test if you could do the `sizeof()` the type directly :sob:. [3 testcases]
  - You don't update your `id_to_struct` mapping when creating a local instance of a `struct` (at least I think - you only seem to assing to it in `FunctionArgumentList::gen_asm()` and `StructType::function_parameter_gen_asm())`), resulting in a crash when you try calling `Context::get_type()` [2 testcases]

- You have a bug in your compiler when dealing with nested function calls. [1 testcase]
  - What I believe is the issue is as follows:
    1. In the outermost function call, you push `a0` to the stack
    2. In the innermost function call, after calling the function, you restore the registers. However, this restoration is done at a scope level rather than a function call level, meaning that you restore the `a0` pushed on the stack by the outermost function , which overwrites the result of the inner function call.
    3. You call the outer function with the restored value of `a0` rather than the result of the inner function call.

- Your pre/post increment/decrement operators aren't implemented correctly, as they don't seem to actually "return" value when used in an assignment operation (e.g. `ret1 = k++`). You only do this when your context is in a `Mode::RETURN` (which I'm assuming is in a return statement), but you should really be doing this all the time, rather than relying on this complicated / hacky `Mode` stuff. [1 testcase]

- You have a bug in your context with `unspill_register()` where you don't properly handle floating point regsiters - after printing the relevant assembly depending on whether or not the `type` of the register is a `FLOAT`, you try to mark the regiser as being used, but call `register_map.at(reg)` which is only valid for non-fp registers.
  - This materialises in test cases with logical or / and operators due to the use of `float_temp_reg`, but I'm not 100% sure what all the cases are where you invoke this behaviour. What's also weird to me is that it only seems to materialise in more complex test cases, and I struggled quite hard in finding the minimum break case.
  - Also happens in `pop_registers()` when you call `FunctionCall::gen_asm()` in `LogicalOr::gen_asm()` (when one of the operands is a function).
  - This seems to affect 3 testcases in total, (none of which use floating points).

- You have a bug in your implementation of pointer arithmetic where you don't consider the case where both operands are pointers (i.e. when calculating the difference between 2 pointers). Your current implementation always multiplies the other operand by `log_size`, which results in the pointer difference being `log_size * log_size` too large. [1 testcase]