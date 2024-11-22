(import srfi-69)

(define (make-opcode-table)
  (define opcodes
    '(sp ssp bp sbp pc rv srv
	 imm
	 ld8 ld16 ld32 ld
	 st8 st16 st32 st
	 dup pop swap over rot
	 add sub div mul mod
	 shr shl sar
	 and or not
	 bitand bitor xor invert
	 gt lt ge le eq neq
	 jmp jz jnz
	 ret call syscall
	 fadd fsub fmul fdiv
	 fge fgt fle flt feq fneq
	 fti itf
	 exit))
  (define (assign-index opcodes idx ret)
    (if (null? opcodes)
	(reverse ret)
	(assign-index (cdr opcodes)
		      (+ 1 idx)
		      (cons (cons (car opcodes) idx) ret))))
  (alist->hash-table (assign-index opcodes 0 '())))

(define opcode-table (make-opcode-table))

(define pseudo-op-table
  (alist->hash-table
   '((rel . (pc add))
     (bpick . ((imm 8) mul bp add ld))
     (spick . ((imm 8) mul sp add ld))
     (bput  . ((imm 8) mul bp add st))
     (sput . ((imm 8) mul bp add st)))))

(define (is-pseudo-op sym)
  (hash-table-ref/default pseudo-op-table sym #f))

(define (is-op sym)
  (hash-table-ref/default opcode-table sym #f))

(define (is-tag sym tag-table)
  (not (or (is-op sym)
	   (is-pseudo-op sym)
	   (not (hash-table-ref/default tag-table sym #f)))))

(define (pseudo-op-pass prog)
  (define (add-to-prog oplist ret)
    (if (null? oplist)
	ret
	(add-to-prog (cdr oplist) (cons (car oplist) ret))))
  (define (impl prog ret)
    (if (null? prog)
	ret
	(let ((next-op (car prog)))
	  (if (is-pseudo-op next-op)
	      (impl (cdr prog) (add-to-prog
				(hash-table-ref pseudo-op-table next-op)
				ret))
	      (impl (cdr prog) (cons (car prog) ret))))))
  (reverse (impl prog '())))

(define (is-tag-op op)
  (and (list? op) (eq 'tag (car op))))

(define (is-imm-op op)
  (and (list? op) (eq 'imm (car op))))

(define (calculate-tag-pos prog)
  (define (impl prog cur-pos tag-table)
    ;; TODO
    '())
  (define tag-table (make-hash-table))
  (impl prog 0 tag-table)
  tag-table)

(define (tag-pass prog)
  ;; TODO
  '())

(define (output-prog prog port)
  ;;
  '())

(define (main input-file output-file)
  (define prog '())
  ;; read s-exp from file
  (with-input-from-file input-file
    (lambda (port) (set! prog (read port))))
  (set! prog (pseudo-op-pass prog))
  (set! prog (tag-pass prog))
  (with-output-to-file output-file
    (lambda (port) (output-prog prog port))))

