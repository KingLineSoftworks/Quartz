# Contribution Guidelines

Pick up an issue, put on some [acid](https://soundcloud.com/intercell/kiki-at-intercell-acid-night-2020?si=ef2408dbdc3146369b2ea2264687329a&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing) or some [ambient dnb](https://soundcloud.com/deep-z-lost-in-heaven/lost-in-heaven-131-dnb-mix-april-2023-atmospheric-liquid-drum-and-bass?si=767ead2f66584ef2a8a62ac51ef785a2&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing), and do good work.

## Merging vs Rebasing

If your branch is out of date with master (or any other branch) please do a rebase instead of a merge. We want a linear commit history, for ease of auditing. If you're not familiar with rebasing, it is not hard. Google is your friend.
Do `git rebase master your_branch` to take all of the commits on `your_branch` and appy them on top of master.

## Style preferences

Currently there is no concrete style guide or autoformatting configured.
Try to adhere to the rest of the source's style.

### Naming

#### Modified Hungarian Notation

When naming variables, use a subset of the hungarian notation.
- `m_` -> member variable
- `p_` -> pointer
- `o_` -> optional
If you have a member variable which is a pointer:
- `mp_`
If you have a member variable which is an optional:
- `mo_`

#### Units

Be explicit about units in the name of your variables. Make sure to also adhere to the modified hungarian notation.
- `speed_mps` -> meters per second
- `delta_ns` -> nanoseconds
- `maxAcceleration_kphps` -> kilometers per hour per second
- `m_currentForce_N` -> newtons (member variable)
- `p_currentTorque_Nm` -> newton meters (pointer)

## Error Handling and Exceptions

These ideas are inspired by Peter Muldoon's [CppCon 2023 talk](https://youtu.be/Oy-VTqz1_58?si=r14vs2eI3zZAV_74).

### When to Use Exceptions

In general, we want to avoid throwing anything if possible.
We should only ever be throwing an exception in rare circumstances (errors which do not happen frequently).

There are two general cases where throwing an exception is okay:
1. the program has encountered an error which is serious enough that getting out of this logic unit with the goal of preventing damange is more important than continuing onwards with execution
1. the program has encountered a situation which cannot be properly dealt with in this logic unit due to lack of context

### Defining Exceptions

We have a templated `RichException` class which contains most of the data you could possibly need.
If you wish to be explicit about your exception type, you can create an alias for the `RichException` type using your desired template parameter.
You should have a strong case for why you wish to extend `RichException` if you choose to.

### Choosing the Correct Exception

Selecting the correct exception type to throw is rather trivial. Use a `RichException` with your desired template parameter.

When you are catching exceptions, you generally should know the types of exceptions your invocations could result in.
If you can reason about this, catch the exceptions which your invocations could result in.
If you cannot reason about this, you are probably better off catching a general `std::runtime_error`.

We do not promote using `std::logic_error`, because it is functionally the same as a `std::runtime_error` but with a worse name.

### General Hygiene

- Throw by value, not by reference
    - Throwing by reference could result in memory issues due to deallocations when unwinding the stack
- Catch by reference
    - We don't want to catch by value because that will result in unnecessary copies and could result in slicing data
    - This will also allow us to modify the data within the object we are catching if we choose to rethrow it with more context

### On `noexcept`

Unless you have a *__really__*, *__very__* strong reason to do so, avoid using the `noexcept` specifier.
Best case, this makes the move constructors and move assignments marginally faster.
Worst case, you cause things to explode if an exception is ever thrown.

## Tests

Make sure your additions are tested. There are examples of how to write unit tests and use the quartz testing framework.
The quartz unit testing framework is defined in the `src/util/unit_test` directory.
Examples on how to use this unit testing framework are in the `test/unit/example` directory.

By default Quartz is set to not compile unit tests. If you wish to compile and run unit tests, ensure to specify the `-DQUARTZ_COMPILE_UNIT_TESTS=True` option when running the cmake command.

### Unit Test Runner Usage Via Command Line

This runner allows you to specify individual test cases from the command line, while still allowing the whole file to be executed via ctest.

To execute a subset of tests within a file via command line, provide the -c flag like so:
```
./bin/ut/test_file_something -c test_case_2
```
You can also specify multiple test cases to run
```
./bin/ut/test_file_something -c test_case_3 test_case_3 test_case_99
```

By default all of the loggers are turned off besides the UNIT_TEST and UT_RUNNER loggers which are set to critical and trace respectively.
If you wish for more verbose logs, you can specify the level of individual loggers via command line arguments with the -l flag like so:
```
./bin/ut/test_another_one -l RIGIDBODY:debug FIELD:trace COLLIDER:error
```

You can also specify test cases and loggers at the same time:
```
./bin/ut/test_hehehe -c test_case_yoyo test_case_lala -l BUFFER:trace IMAGE:critical MODEL:info
./bin/ut/test_hehehe -l DOODAD:trace INPUTMAN:critical -c test_case_yoyo_2 test_case_lala_the_sequel
```
