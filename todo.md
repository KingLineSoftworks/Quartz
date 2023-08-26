# to do list
- [ ] better initialization of loggers
    - remove boiler plate logger.hpp file for each library
    - declare loggers in bulk and declare which logger you are using
        - declare many loggers in one statement
            - allow statement to determine level upon logger declaration
        - one big logger.hpp file per section of code
            - allow for repeated use of statement
                - util loggers
                - quartz loggers
                - application loggers
            - include this logger file to get knowledge of all relevant declared loggers
                - classes can use a "use_logger" statement or something similar to determine which logger they are going to be using
                