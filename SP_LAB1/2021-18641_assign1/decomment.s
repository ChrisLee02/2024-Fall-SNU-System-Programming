	.file	"decomment.c"
	.text
	.globl	handleStateTransition
	.type	handleStateTransition, @function
handleStateTransition:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, %eax
	movb	%al, -24(%rbp)
	movsbl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	transitionState
	movl	%eax, -4(%rbp)
	movsbl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	executeAction
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	handleStateTransition, .-handleStateTransition
	.globl	transitionState
	.type	transitionState, @function
transitionState:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, %eax
	movb	%al, -8(%rbp)
	cmpl	$8, -4(%rbp)
	ja	.L4
	movl	-4(%rbp), %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L6(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L6(%rip), %rdx
	addq	%rdx, %rax
	notrack jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L6:
	.long	.L14-.L6
	.long	.L13-.L6
	.long	.L12-.L6
	.long	.L11-.L6
	.long	.L10-.L6
	.long	.L9-.L6
	.long	.L8-.L6
	.long	.L7-.L6
	.long	.L5-.L6
	.text
.L14:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromNormalState
	jmp	.L15
.L13:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromSlashReceiveInitially
	jmp	.L15
.L12:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromInsideOneLineComment
	jmp	.L15
.L11:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromInsideMultiLineComment
	jmp	.L15
.L10:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromAsteriskReceivedInitiallyInComment
	jmp	.L15
.L9:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromInsideSingleQuote
	jmp	.L15
.L8:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromInsideDoubleQuote
	jmp	.L15
.L7:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromEscapeSeqInSingleQuote
	jmp	.L15
.L5:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	transitionFromEscapeSeqInDoubleQuote
	jmp	.L15
.L4:
	movl	$0, %eax
.L15:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	transitionState, .-transitionState
	.globl	executeAction
	.type	executeAction, @function
executeAction:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, %eax
	movb	%al, -8(%rbp)
	cmpl	$8, -4(%rbp)
	ja	.L29
	movl	-4(%rbp), %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L19(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L19(%rip), %rdx
	addq	%rdx, %rax
	notrack jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L19:
	.long	.L27-.L19
	.long	.L26-.L19
	.long	.L25-.L19
	.long	.L24-.L19
	.long	.L23-.L19
	.long	.L22-.L19
	.long	.L21-.L19
	.long	.L20-.L19
	.long	.L18-.L19
	.text
.L27:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromNormalState
	jmp	.L28
.L26:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromSlashReceiveInitially
	jmp	.L28
.L25:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromInsideOneLineComment
	jmp	.L28
.L24:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromInsideMultiLineComment
	jmp	.L28
.L23:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromAsteriskReceivedInitiallyInComment
	jmp	.L28
.L22:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromInsideSingleQuote
	jmp	.L28
.L21:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromInsideDoubleQuote
	jmp	.L28
.L20:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromEscapeSeqInSingleQuote
	jmp	.L28
.L18:
	movsbl	-8(%rbp), %eax
	movl	%eax, %edi
	call	executeActionFromEscapeSeqInDoubleQuote
	jmp	.L28
.L29:
	nop
.L28:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	executeAction, .-executeAction
	.globl	transitionFromNormalState
	.type	transitionFromNormalState, @function
transitionFromNormalState:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L31
	movl	$1, %eax
	jmp	.L32
.L31:
	cmpb	$39, -4(%rbp)
	jne	.L33
	movl	$5, %eax
	jmp	.L32
.L33:
	cmpb	$34, -4(%rbp)
	jne	.L34
	movl	$6, %eax
	jmp	.L32
.L34:
	movl	$0, %eax
.L32:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	transitionFromNormalState, .-transitionFromNormalState
	.globl	transitionFromSlashReceiveInitially
	.type	transitionFromSlashReceiveInitially, @function
transitionFromSlashReceiveInitially:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L36
	movl	$2, %eax
	jmp	.L37
.L36:
	cmpb	$42, -4(%rbp)
	jne	.L38
	movl	$3, %eax
	jmp	.L37
.L38:
	movl	$0, %eax
.L37:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	transitionFromSlashReceiveInitially, .-transitionFromSlashReceiveInitially
	.globl	transitionFromInsideOneLineComment
	.type	transitionFromInsideOneLineComment, @function
transitionFromInsideOneLineComment:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L40
	movl	$0, %eax
	jmp	.L41
.L40:
	movl	$2, %eax
.L41:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	transitionFromInsideOneLineComment, .-transitionFromInsideOneLineComment
	.globl	transitionFromInsideMultiLineComment
	.type	transitionFromInsideMultiLineComment, @function
transitionFromInsideMultiLineComment:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$42, -4(%rbp)
	jne	.L43
	movl	$4, %eax
	jmp	.L44
.L43:
	movl	$3, %eax
.L44:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	transitionFromInsideMultiLineComment, .-transitionFromInsideMultiLineComment
	.globl	transitionFromAsteriskReceivedInitiallyInComment
	.type	transitionFromAsteriskReceivedInitiallyInComment, @function
transitionFromAsteriskReceivedInitiallyInComment:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L46
	movl	$0, %eax
	jmp	.L47
.L46:
	cmpb	$42, -4(%rbp)
	jne	.L48
	movl	$4, %eax
	jmp	.L47
.L48:
	movl	$3, %eax
.L47:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	transitionFromAsteriskReceivedInitiallyInComment, .-transitionFromAsteriskReceivedInitiallyInComment
	.globl	transitionFromInsideSingleQuote
	.type	transitionFromInsideSingleQuote, @function
transitionFromInsideSingleQuote:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$39, -4(%rbp)
	jne	.L50
	movl	$0, %eax
	jmp	.L51
.L50:
	cmpb	$92, -4(%rbp)
	jne	.L52
	movl	$7, %eax
	jmp	.L51
.L52:
	movl	$5, %eax
.L51:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	transitionFromInsideSingleQuote, .-transitionFromInsideSingleQuote
	.globl	transitionFromInsideDoubleQuote
	.type	transitionFromInsideDoubleQuote, @function
transitionFromInsideDoubleQuote:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$34, -4(%rbp)
	jne	.L54
	movl	$0, %eax
	jmp	.L55
.L54:
	cmpb	$92, -4(%rbp)
	jne	.L56
	movl	$8, %eax
	jmp	.L55
.L56:
	movl	$6, %eax
.L55:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	transitionFromInsideDoubleQuote, .-transitionFromInsideDoubleQuote
	.globl	transitionFromEscapeSeqInSingleQuote
	.type	transitionFromEscapeSeqInSingleQuote, @function
transitionFromEscapeSeqInSingleQuote:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movl	$5, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	transitionFromEscapeSeqInSingleQuote, .-transitionFromEscapeSeqInSingleQuote
	.globl	transitionFromEscapeSeqInDoubleQuote
	.type	transitionFromEscapeSeqInDoubleQuote, @function
transitionFromEscapeSeqInDoubleQuote:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movl	$6, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	transitionFromEscapeSeqInDoubleQuote, .-transitionFromEscapeSeqInDoubleQuote
	.globl	printToStdout
	.type	printToStdout, @function
printToStdout:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movq	stdout(%rip), %rdx
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fputc@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	printToStdout, .-printToStdout
	.globl	executeActionFromNormalState
	.type	executeActionFromNormalState, @function
executeActionFromNormalState:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	je	.L65
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
	jmp	.L62
.L65:
	nop
.L62:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	executeActionFromNormalState, .-executeActionFromNormalState
	.globl	executeActionFromSlashReceiveInitially
	.type	executeActionFromSlashReceiveInitially, @function
executeActionFromSlashReceiveInitially:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$47, -4(%rbp)
	jne	.L67
	movl	$32, %edi
	call	printToStdout
	jmp	.L66
.L67:
	cmpb	$42, -4(%rbp)
	jne	.L69
	movl	$32, %edi
	call	printToStdout
	jmp	.L66
.L69:
	movl	$47, %edi
	call	printToStdout
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
.L66:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	executeActionFromSlashReceiveInitially, .-executeActionFromSlashReceiveInitially
	.globl	executeActionFromInsideOneLineComment
	.type	executeActionFromInsideOneLineComment, @function
executeActionFromInsideOneLineComment:
.LFB15:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L72
	movl	$10, %edi
	call	printToStdout
.L72:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	executeActionFromInsideOneLineComment, .-executeActionFromInsideOneLineComment
	.globl	executeActionFromInsideMultiLineComment
	.type	executeActionFromInsideMultiLineComment, @function
executeActionFromInsideMultiLineComment:
.LFB16:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L75
	movl	$10, %edi
	call	printToStdout
.L75:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	executeActionFromInsideMultiLineComment, .-executeActionFromInsideMultiLineComment
	.globl	executeActionFromAsteriskReceivedInitiallyInComment
	.type	executeActionFromAsteriskReceivedInitiallyInComment, @function
executeActionFromAsteriskReceivedInitiallyInComment:
.LFB17:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	cmpb	$10, -4(%rbp)
	jne	.L78
	movl	$10, %edi
	call	printToStdout
.L78:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	executeActionFromAsteriskReceivedInitiallyInComment, .-executeActionFromAsteriskReceivedInitiallyInComment
	.globl	executeActionFromInsideSingleQuote
	.type	executeActionFromInsideSingleQuote, @function
executeActionFromInsideSingleQuote:
.LFB18:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	executeActionFromInsideSingleQuote, .-executeActionFromInsideSingleQuote
	.globl	executeActionFromInsideDoubleQuote
	.type	executeActionFromInsideDoubleQuote, @function
executeActionFromInsideDoubleQuote:
.LFB19:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	executeActionFromInsideDoubleQuote, .-executeActionFromInsideDoubleQuote
	.globl	executeActionFromEscapeSeqInSingleQuote
	.type	executeActionFromEscapeSeqInSingleQuote, @function
executeActionFromEscapeSeqInSingleQuote:
.LFB20:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	executeActionFromEscapeSeqInSingleQuote, .-executeActionFromEscapeSeqInSingleQuote
	.globl	executeActionFromEscapeSeqInDoubleQuote
	.type	executeActionFromEscapeSeqInDoubleQuote, @function
executeActionFromEscapeSeqInDoubleQuote:
.LFB21:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	movsbl	-4(%rbp), %eax
	movl	%eax, %edi
	call	printToStdout
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	executeActionFromEscapeSeqInDoubleQuote, .-executeActionFromEscapeSeqInDoubleQuote
	.section	.rodata
	.align 8
.LC0:
	.string	"Error: line %d: unterminated comment\n"
	.text
	.globl	printErrorLog
	.type	printErrorLog, @function
printErrorLog:
.LFB22:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	stderr(%rip), %rax
	movl	-4(%rbp), %edx
	leaq	.LC0(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	printErrorLog, .-printErrorLog
	.globl	main
	.type	main, @function
main:
.LFB23:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$-1, -8(%rbp)
	movl	$1, -20(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	$0, -12(%rbp)
.L90:
	call	getchar@PLT
	movl	%eax, -4(%rbp)
	cmpl	$-1, -4(%rbp)
	je	.L94
	movl	-4(%rbp), %eax
	movb	%al, -21(%rbp)
	movl	-4(%rbp), %eax
	movsbl	%al, %edx
	movl	-12(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	handleStateTransition
	movl	%eax, -12(%rbp)
	cmpl	$3, -12(%rbp)
	jne	.L87
	movl	-16(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jne	.L87
	movl	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
.L87:
	cmpl	$0, -12(%rbp)
	jne	.L88
	movl	-16(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	.L88
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
.L88:
	cmpb	$10, -21(%rbp)
	jne	.L90
	addl	$1, -20(%rbp)
	jmp	.L90
.L94:
	nop
	movl	-16(%rbp), %eax
	cmpl	-8(%rbp), %eax
	je	.L91
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	printErrorLog
	movl	$1, %eax
	jmp	.L92
.L91:
	movl	$0, %eax
.L92:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
