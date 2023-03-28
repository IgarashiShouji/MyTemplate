#include "mruby.h"
#include "mruby/mruby_mystub/mruby_mystub.h"
#include "mystub.h"
#include <stdio.h>

static mrb_value mystub_etst2(mrb_state * mrb, mrb_value self)
{
    test3("Hello World!");
    printf("test2 method!\n");
    {
        int no;
        for(no = 0; no < CodeNo_End; no ++)
        {
            printf("  %2d: test2\n", no);
        }
    }
    return self;
}

void mrb_mruby_mystub_gem_init(mrb_state * mrb)
{
    struct RClass * my_stub;

    printf("%s,(%d)\n", __FILE__, __LINE__);
    my_stub = mrb_define_class(mrb, "MyStub", mrb->object_class);
    mrb_define_method(mrb, my_stub, "test2", mystub_etst2, MRB_ARGS_NONE());
    printf("%s,(%d)\n", __FILE__, __LINE__);
}
void mrb_mruby_mystub_gem_final(mrb_state * mrb)
{
    printf("%s,(%d)\n", __FILE__, __LINE__);
}
