/*
all physical memory is statically mapped, and then a virtual memory manager 
has control of the rest of the address space, primarily for stacks but also 
mmio and swapping and etc. - if a machine has 4GB+ physical, then reserve 10% 
if PAE isn't supported, otherwise enable PAE. if a machine has more than 64GB 
then reserve 10% if 64-bit isn't supported, otherwise use long mode
*/