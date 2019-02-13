#include<iostream>
#include<stdint.h>

void DoSomethingInterestingWithMyAPI(const uint8_t *Data, size_t Size) {
    char buf[30];
    if ( Data[0] == 'p' && Data[1] == 'i' && Data[2] == 'n' && Data[3] == 'g' ) {
        __builtin_trap();
    }
    return ;
}

// fuzz_target.cc
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  // DoSomethingInterestingWithMyAPI(Data, Size);
  return 0;  // Non-zero return values are reserved for future use.
}
