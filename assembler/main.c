// telluric client application assembler
// defines assembler syntax and instruction encoding by implementation

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "opts.h"
#include "file.h"

#include "literal.h"

int main(int argc, char* argv[]) {


  printf("tests\n");
  printf("exit: %d\n", parse_opcode("exit", 4));

  opts o = parse_opts(argc, argv);

  asm_file f = load_file(o.in_file);
  f = strip_nonsemantic(f);

  for (int i = 0; i < f.lines_count; i++) {
    printf("%d: ", i);
    puts(f.lines[i].str);
  }

  sectioned_file s = parse_sections(f);
  reduced_file r = reduce_file(s);

  data_domains d = identify_data(r.data);

  object* objs = calloc(r.code_count, sizeof(*objs));

  for (int i = 0; i < r.code_count; i++) {
    objs[i] = compile_code(r.code[i], d);
  }

  data_object* dats = calloc(r.data.defs_count, sizeof(*dats));

  for (int i = 0; i < r.data.defs_count; i++) {
    dats[i] =
      compile_data(get_label_id(d, r.data.defs[i].name, strlen(r.data.defs[i].name)), r.data.defs[i]);
  }

  program_file p = compile_file(d, r.meta, objs, r.code_count, dats, r.data.defs_count);

  int len;
  byte* flattened = collapse_file(p, &len);

  printf("writing file\n");

  FILE* out = fopen(o.out_file, "w");

  if (out == NULL) {
    printf("could not open %s for writing\n", o.out_file);
    exit(1);
  }

  fwrite(flattened, len, 1, out);

  return 0;
}
