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

fact:
  move $s0, $sp
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  lw $t0, 0($sp)
  li $t1, 1
  beq $t0, $t1, label1
  j label2
label1:
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  move $v0, $t0
  jr $ra
  j label3
label2:
  lw $t2, 0($sp)
  sub $t1, $t2, 1
  move $a0, $t1
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  addi $sp, $sp, -4
  sw $t2, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t2, 0($sp)
  addi $sp, $sp, 4
  lw $t1, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  lw $t4, 0($sp)
  mul $t3, $t4, $v0
  addi $sp, $sp, 4
  move $v0, $t3
  jr $ra
label3:

main:
  move $s0, $sp
  addi $sp, $sp, -4
  addi $sp, $sp, -4
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 4($sp)
  lw $t0, 4($sp)
  li $t1, 1
  bge $t0, $t1, label4
  j label5
label4:
  lw $t0, 4($sp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t0, 0($sp)
  addi $sp, $sp, 4
  sw $v0, 0($sp)
  j label6
label5:
  li $t0, 1
  sw $t0, 0($sp)
label6:
  lw $t0, 0($sp)
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
  addi $sp, $sp, 8
  move $v0, $t0
  jr $ra
