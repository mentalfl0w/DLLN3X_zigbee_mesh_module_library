# DLLN3X_zigbee_mesh_module_library
This library allows you to use DLLN3X ZigBee mesh module very easily.

This library is now complete, the remaining work is mainly maintenance and bug fixes, welcome feedback by opening issues.

# Available Features

1. Basic message sending and receiving.
2. Module address reading.
3. On-chip red led flashing control.
4. Configuration reading, modification (baud rate, address, etc.).
5. Error Report.
6. Link quality test.
7. On-chip pin control.

# Example run results

![example](./imgs/example.png)

# Change Log

**V1.1.4 2024.2.1**

+ Fix the bug of vector may be accessed out of range.

**V1.1.3 2023.5.3**

+ Bug fix, use minimized vector implementation instead of std::vector to adapt to more boards, the implementation is from [@a6c0424fa083's vector_for_arduino](https://github.com/a6c0424fa083/vector_for_arduino), which I improved slightly on.

**V1.1.2 2023.4.22**

+ Modify header files to avoid compilation conflicts, add warnings for certain boards that do not support STL.(If your board are Arduino AVR architecture(like Arduino Uno), you have to comment 'const std::nothrow_t std::nothrow = { };' in 'ArduinoSTL/src/new_handler.cpp')

**V1.1.1 2023.3.18**

+ Fix the bug of incomplete copy.

**V1.1.0 2023.3.18**

+ Completely rewritten.
+ Join the ZigbeeFrame container.
+ Greatly improve stability.
+ Supports configuration reading and modification, link quality testing, on-board pin control, error reporting, etc.

**V1.0.4 2023.1.26**

- Bug fixes and stability improvements.

**V1.0.3 2023.1.12**

- Bug fixed for example.

**V1.0.2 2022.12.8**

- Bug fixed.
- Add support for SoftwareSerial.

**V1.0.1 2022.9.8**

- Bug fixed.
- Code readability improvement.
- Provide more appropriate examples and interfaces.

**V1.0.0 2022.9.7**

- Initial code, a lot of things need to be improved.

