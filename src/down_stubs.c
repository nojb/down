/*---------------------------------------------------------------------------
   Copyright (c) 2019 The down programmers. All rights reserved.
   SPDX-License-Identifier: ISCe.
   %%NAME%% release %%VERSION%%
   --------------------------------------------------------------------------*/

#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>

CAMLprim value ocaml_down_stdin_set_raw_mode (value set_raw)
{
  CAMLparam1 (set_raw);
  static bool is_raw = false;
  static struct termios restore = {0};
  struct termios set;

  if (Bool_val (set_raw)) {
    if (!is_raw) {
      if (!isatty (0)) { CAMLreturn (Val_bool (0)); }
      if (tcgetattr (0, &restore) < 0) { CAMLreturn (Val_bool (0)); }
      set = restore;
      set.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
      set.c_oflag &= ~(OPOST);
      set.c_cflag |= (CS8);
      set.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
      set.c_cc[VMIN] = 1; set.c_cc[VTIME] = 0;
      if (tcsetattr (0, TCSAFLUSH, &set) < 0) { CAMLreturn (Val_bool (0)); }
      else { is_raw = true; };
    }
  } else {
    if (is_raw) {
      if (tcsetattr (0, TCSAFLUSH, &restore) < 0) { CAMLreturn (Val_bool (0)); }
      else { is_raw = false; }
    }
  }
  CAMLreturn (Val_bool (1));
}

CAMLprim value ocaml_down_stdin_readc (value unit)
{
  CAMLparam1 (unit);
  int ret; unsigned char buf;
  ret = read(0, &buf, 1);
  if (ret == 1) { CAMLreturn (Val_int (buf)); };
  if (ret == 0) { CAMLreturn (Val_int (-1)); };
  if (ret == -1 && errno == EINTR) { CAMLreturn (Val_int (-2)); };
  CAMLreturn (Val_int (-3));
}

CAMLprim value ocaml_down_sigwinch (value unit)
{
  CAMLparam1 (unit);
  CAMLreturn (Val_int (SIGWINCH));
}
