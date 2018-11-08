// $Id: dstack.h,v 1.2 2003/04/14 05:51:03 mbridak Exp $
// dstack.h -- Dynamic stack (DStack) declaration and function prototypes.
//

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef struct
{
   float value;
   void *next, *previous;
} STACKVALUE;

class DStack
{
  private:
    STACKVALUE *bottom_;
    STACKVALUE *top_;
    int number_items;

  public:
    DStack();
    ~DStack();
    void clear_stack();    // erase everything
    int push(float val);
    int num_items();
    float pop();
    int empty();
    void print();
};
