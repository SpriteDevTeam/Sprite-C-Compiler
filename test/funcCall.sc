//int add(int a, int b);

char c = 'f';
int a = 10;

int add(int a, int b) {
  int c = a += b;
  return a;
}

/*
-----  0 <--- current s0
 ra
----- -4
 previous s0 (bp/fp)
----- -8
 a
----- -12
 b
----- -16
 c
----- -20 <--- current sp
*/



int main() {
    int c = 1+add(1, 2);
    return 1+1;
}

/*
main:
  # prologue
  addi sp,sp,-12  # return address + old base pointer + one local variable space
  sw ra,8(sp)
  sw s0,4(sp)
  add s0,sp,12

  # stack machine begin (c = 1+add(1,2))
  addi sp,sp,-4   # ---
  li t1,1         #  | push (int)1
  sw t1,0(sp)     # ---
  addi sp,sp,-4   # ---
  li a0,1         #  | call add() function
  li a1,2         #  | and
  call add        #  | push return value of add(1,2)
  sw a0,0(sp)     # ---
  lw t1,4(sp)     # ---
  lw t2,0(sp)     #  | pop 2 oprand push 1 result
  addi sp,sp,4    #  | and
  add t1,t1,t2    #  | push result
  sw t1,0(sp)     # ---
  lw t1,0(sp)     # ---
  sw t1,-12(s0)   #  | c = result
                  # ---
  addi sp,sp,4    # --- end of stmt, pop the last result, stack machine end

  # stack machine begin (return 1+1)
  addi sp,sp,-4
  li t1,1
  sw t1,0(sp) # push 1
  addi sp,sp,-4
  li t1,1
  sw t1,0(sp)
  lw t1,4(sp)
  lw t2,0(sp)
  addi sp,sp,4 # pop 2 oprand push 1 result
  add t1,t1,x2
  sw t1,0(sp) # push result
  lw a0,0(sp)
  addi sp,sp,4 # end of return stmt, stack machine end

  # epilogue
  lw ra,8(sp)
  lw s0,4(sp)
  addi sp,sp,12
  jr ra
*/
