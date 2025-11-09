/*
  Copyright 2025 Osfabias

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/*
  @file src/terminal_styles.h
  @brief Terminal style macros
  @author Ilya Buravov (ilburale@gmail.com)
*/

#pragma once

/* Reset */
#define STYLE_RESET "\x1b[0m"

/* Font */
#define BOLD "\x1b[1m"
#define DIM  "\x1b[2m"

/* 8-color foreground */
#define FG_BLACK   "\x1b[30m"
#define FG_RED     "\x1b[31m"
#define FG_GREEN   "\x1b[32m"
#define FG_YELLOW  "\x1b[33m"
#define FG_BLUE    "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN    "\x1b[36m"
#define FG_WHITE   "\x1b[37m"

/* Bright foreground */
#define FG_BBLACK   "\x1b[90m"
#define FG_BRED     "\x1b[91m"
#define FG_BGREEN   "\x1b[92m"
#define FG_BYELLOW  "\x1b[93m"
#define FG_BBLUE    "\x1b[94m"
#define FG_BMAGENTA "\x1b[95m"
#define FG_BCYAN    "\x1b[96m"
#define FG_BWHITE   "\x1b[97m"

/* 8-color background */
#define BG_BLACK   "\x1b[40m"
#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE   "\x1b[47m"

/* Bright background */
#define BG_BBLACK   "\x1b[100m"
#define BG_BRED     "\x1b[101m"
#define BG_BGREEN   "\x1b[102m"
#define BG_BYELLOW  "\x1b[103m"
#define BG_BBLUE    "\x1b[104m"
#define BG_BMAGENTA "\x1b[105m"
#define BG_BCYAN    "\x1b[106m"
#define BG_BWHITE   "\x1b[107m"

