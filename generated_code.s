.section .data
    x: .quad 0                  # Define x as a quad-word (8 bytes)
    message1: .asciz "teste\n"  # First message
    message2: .asciz "Oi\n"     # Second message
    format_input: .asciz "%d"   # Format string for scanf

.section .text
.globl _start
_start:
    # Assign constant to variable x
    movq $5, x(%rip)            # Use 'movq' for quad-word (64-bit)

    # Print first string
    lea message1(%rip), %rdi    # Load address of message1 into %rdi
    xor %rax, %rax              # Clear %rax (required for printf)
    call printf                 # Call printf

    # Print second string
    lea message2(%rip), %rdi    # Load address of message2 into %rdi
    xor %rax, %rax              # Clear %rax (required for printf)
    call printf                 # Call printf

    # Input integer
    lea x(%rip), %rsi           # Load address of x into %rsi (second argument)
    lea format_input(%rip), %rdi # Load address of format_input into %rdi (first argument)
    xor %rax, %rax              # Clear %rax (required for scanf)
    call scanf                  # Call scanf

    # Exit program
    mov $60, %rax               # syscall number for exit
    xor %rdi, %rdi              # Exit code 0
    syscall                     # Make the syscall
