//
// Created by Timur Akhmetzianov on 22.11.2024
//

#pragma once

#include "PRNGenerator.h"


class Interpreter {
 public:
    explicit Interpreter(PRNGenerator *generator) : generator_(generator) {}

 private:
    PRNGenerator *generator_;
};
