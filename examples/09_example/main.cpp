/*
  This file is part of KDBindings.

  SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: John Doe <john.doe@example.com>

  SPDX-License-Identifier: MIT

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <kdbindings/signal.h>

#include <iostream>
#include <string>

using namespace KDBindings;

int main()
{
    // Create a new signal that carries two different types
    Signal<std::string, double> signal;

    // Connect a lambda to the signal
    (void)signal.connect([](const std::string &text, double number) {
        std::cout << "First handler says: " << text << " " << number << std::endl;
    });

    // Connect another lambda to the same signal
    (void)signal.connect([](const std::string &text, double number) {
        std::cout << "Second handler also got: " << text << " " << number << std::endl;
    });

    // Emit the signal; both connected lambdas will be called
    signal.emit("Pi approximately equals", 3.14159);

    return 0;
}
