(import srfi-69)
(import matchable)
(import (chicken io))
(import (chicken condition))
(import (chicken keyword))
(import (chicken process-context))
(import (chicken port))
(import (chicken blob))
(import srfi-4)

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

(define (is-pseudo-op? sym)
  (hash-table-ref/default pseudo-op-table sym #f))

(define (is-op? sym)
  (hash-table-ref/default opcode-table sym #f))

(define (is-tag? sym tag-table)
  (not (or (is-op? sym)
	   (is-pseudo-op? sym)
	   (not (hash-table-ref/default tag-table sym #f)))))

(define (preprocess prog)
  (let loop ((lst prog) (ret '()))
    (match lst
      (() (reverse ret))
      (('imm num . r) (loop r (cons (list 'imm num) ret)))
      ((x . r) (if (keyword? x)
                   (loop r
                         (cons (list 'tag
                                     (string->symbol (keyword->string x)))
                               ret))
                   (loop r (cons x ret)))))))

(define (pseudo-op-pass prog)
  (define (add-to-prog oplist ret)
    (if (null? oplist)
	ret
	(add-to-prog (cdr oplist) (cons (car oplist) ret))))
  (define (impl prog ret)
    (if (null? prog)
	ret
	(let ((next-op (car prog)))
	  (if (is-pseudo-op? next-op)
	      (impl (cdr prog) (add-to-prog
				(hash-table-ref pseudo-op-table next-op)
				ret))
	      (impl (cdr prog) (cons (car prog) ret))))))
  (reverse (impl prog '())))

(define (is-tag-op? op)
  (and (list? op) (eq? 'tag (car op))))

(define (is-imm-op? op)
  (and (list? op) (eq? 'imm (car op))))

(define (tag-preprocess-pass prog)
  (define (is-local-tag? sym)
    (eq? #\. (string-ref (symbol->string sym) 0)))
  (define (compose-tag seg sym)
    (if (null? seg)
	sym
	(if (is-local-tag? sym)
	    (string->symbol (string-append (symbol->string seg) (symbol->string sym)))
	    sym)))
  (define (not-op? sym) (not (or (is-op? sym) (is-pseudo-op? sym))))
  (define (impl prog ret curseg)
    (if (null? prog)
	(reverse ret)
	(let ((cur (car prog)))
	  (cond ((symbol? cur) (if (not-op? cur)
				   (impl (cdr prog)
					 (cons (compose-tag curseg cur) ret)
					 curseg)
				   (impl (cdr prog) (cons cur ret) curseg)))
		((is-tag-op? cur) (if (is-local-tag? (cadr cur))
				     (impl (cdr prog)
					   (cons (list 'tag (compose-tag curseg (cadr cur))) ret)
					   curseg)
				     (impl (cdr prog) (cons cur ret) (cadr cur))))
		(else (impl (cdr prog) (cons cur ret) curseg))))))
  (impl prog '() '()))

(define (calculate-tag-pos prog)
  (define tag-table (make-hash-table))
  (define (impl prog cur-pos)
    (if (null? prog)
	'()
	(let ((cur-op (car prog)))
	  (cond ((is-tag-op? cur-op) (let ()
				       (hash-table-set! tag-table (cadr cur-op) cur-pos)
				       (impl (cdr prog) cur-pos)))
		((is-op? cur-op) (impl (cdr prog) (+ 1 cur-pos)))
		(else (impl (cdr prog) (+ 9 cur-pos)))))))
  (impl prog 0)
  tag-table)

(define (tag-pass prog)
  (define tag-table (calculate-tag-pos prog))
  (define (impl prog ret cur-pos)
    (if (null? prog)
	(reverse ret)
	(let ((cur-op (car prog)))
	  (define do-nothing
	    (lambda (op-size)
	      (impl (cdr prog) (cons cur-op ret) (+ op-size cur-pos))))
	  (cond ((and (symbol? cur-op) (not (is-op? cur-op)))
		 (let ((sym-pos (hash-table-ref/default tag-table cur-op #f)))
		   (if sym-pos
		       (impl (cdr prog)
			     (cons (list 'imm (+ -9 (- sym-pos cur-pos))) ret)
			     (+ 9 cur-pos))
		       `(error "symbol not found:" ,cur-op)))) ;; error
		((is-tag-op? cur-op) (impl (cdr prog) ret cur-pos))
		((is-op? cur-op) (do-nothing 1))
		(else (do-nothing 9))))))
  (impl prog '() 0))

(define (output-prog prog output-file)
  (define (write-number x port)
    (cond ((or (rational? x)
               (real? x))    (write-string
                              (blob->string (f64vector->blob (f64vector x)))
                              #f
                              port))
          ((integer? x) (write-string (blob->string
                                       (u64vector->blob (u64vector x)))
                                      #f
                                      port))
          (else (abort "Expect a number"))))
  (call-with-output-file output-file
    (lambda (port)
      (let loop ((prog prog))
        (if (null? prog)
            '()
            (let ((cur (car prog)))
              (match cur
                (('imm x)
                 (let ()
                   (write-byte (hash-table-ref opcode-table 'imm) port)
                   (write-number x port)
                   (loop (cdr prog))))
                (x
                 (let ()
                   (write-byte (hash-table-ref opcode-table x) port)
                   (loop (cdr prog)))))))))))

(define (main input-file output-file)
  (define prog '())
  ;; read s-exp from file
  (define input-str
    (with-input-from-file input-file
      read-string))
  (call-with-input-string (string-append "(" input-str ")")
    (lambda (port) (set! prog (read port))))
  (set! prog (preprocess prog))
  (set! prog (pseudo-op-pass prog))
  (set! prog (tag-preprocess-pass prog))
  (let ((new-prog (tag-pass prog)))
    (if (or (null? new-prog)
	    (eq? 'error (car prog)))
	(display new-prog) ;; error
	(output-prog new-prog output-file))))

(define args (argv))

(if (not (= (length args) 3))
    (let ()
      (display "Usage: fvm-as <input-file> <output-file>")
      (newline)
      (exit))
    '())

(main (cadr args) (caddr args))
