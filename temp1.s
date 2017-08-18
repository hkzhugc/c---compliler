.data
_prompt: .asciiz "Enter an integer: "
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  move $s0, $sp
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  addi $sp, $sp, -4
  li $t0, 0
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  li $t1, 1
  sw $t1, 0($sp)
  addi $sp, $sp, -4
  li $t2, 0
  sw $t2, 0($sp)
  addi $sp, $sp, -4
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 0($sp)
label1:
  lw $t0, 4($sp)
  lw $t1, 0($sp)
  blt $t0, $t1, label2
  j label3
label2:
  lw $t1, 12($sp)
  lw $t2, 8($sp)
  add $t0, $t1, $t2
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  lw $t0, 12($sp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 12($sp)
  sw $t0, 16($sp)
  lw $t0, 0($sp)
  sw $t0, 12($sp)
  lw $t1, 8($sp)
  addi $t0, $t1, 1
  sw $t0, 8($sp)
  addi $sp, $sp, 4
  j label1
label3:
  lw $t0, 12($sp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  li $t0, 0
  addi $sp, $sp, 24
  move $sp, $s0
  jr $ra
