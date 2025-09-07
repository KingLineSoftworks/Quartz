# Contribution Guidelines

Pick up an issue, put on some [acid](https://soundcloud.com/intercell/kiki-at-intercell-acid-night-2020?si=ef2408dbdc3146369b2ea2264687329a&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing) or some [ambient dnb](https://soundcloud.com/deep-z-lost-in-heaven/lost-in-heaven-131-dnb-mix-april-2023-atmospheric-liquid-drum-and-bass?si=767ead2f66584ef2a8a62ac51ef785a2&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing), and do good work.

## Merging vs Rebasing

If your branch is out of date with master (or any other branch) please do a rebase instead of a merge. We want a linear commit history, for ease of auditing. If you're not familiar with rebasing, it is not hard. Google is your friend.
Do `git rebase master your_branch` to take all of the commits on `your_branch` and appy them on top of master.

## Style preferences

Currently there is no concrete style guide or autoformatting configured.
Try to adhere to the rest of the source's style.

## Errors and Exceptions

Todo: Fill this out.

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
